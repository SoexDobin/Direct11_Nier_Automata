#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Header Parser for Prefab Metadata Generation
Client/public/*.h 를 파싱하여 client_metadata.json 생성
"""

import sys
import os
import re
import json
import argparse
from pathlib import Path
from typing import Dict, List, Optional


def print_header():
    print("=" * 40)
    print("Header Parser — Prefab Metadata Generator")
    print("=" * 40)
    print()


def detect_namespace(content: str) -> Optional[str]:
    """NS_BEGIN(Xxx) 또는 namespace Xxx { 감지"""
    m = re.search(r'NS_BEGIN\s*\(\s*(\w+)\s*\)', content)
    if m:
        return m.group(1)
    m = re.search(r'namespace\s+(\w+)\s*\{', content)
    if m:
        return m.group(1)
    return None


def extract_base_class(class_line: str) -> Optional[str]:
    """상속 관계에서 첫 번째 public 베이스 클래스 추출"""
    m = re.search(r':\s*public\s+(\w+)', class_line)
    if m:
        return m.group(1)
    return None


def extract_members(content: str, class_start: int) -> List[Dict]:
    """
    클래스 본문에서 멤버 변수를 추출.
    Type m_Name = {default}; 또는 Type m_Name; 패턴
    """
    members = []

    # 클래스 본문 추출 (여는 { 부터 매칭되는 } 까지)
    brace_depth = 0
    body_start = content.find('{', class_start)
    if body_start == -1:
        return members

    body = ""
    for i in range(body_start, len(content)):
        c = content[i]
        if c == '{':
            brace_depth += 1
        elif c == '}':
            brace_depth -= 1
            if brace_depth == 0:
                body = content[body_start + 1:i]
                break

    if not body:
        return members

    # 접근 제어자 기반으로 현재 access 추적
    current_access = "private"
    lines = body.split('\n')

    for line in lines:
        stripped = line.strip()

        # 접근 제어자 변경
        if stripped.startswith("public:"):
            current_access = "public"
            continue
        elif stripped.startswith("private:"):
            current_access = "private"
            continue
        elif stripped.startswith("protected:"):
            current_access = "protected"
            continue

        # 빈 줄, 주석, 함수 선언 건너뛰기
        if not stripped or stripped.startswith("//") or stripped.startswith("/*"):
            continue
        if '(' in stripped:  # 함수 선언
            continue
        if stripped.startswith("using "):
            continue
        if stripped.startswith("NO_COPY") or stripped.startswith("DECLARE_"):
            continue

        # 멤버 변수 패턴: Type m_Name = {default};
        # 또는: Type m_Name;
        # 또는: Shared<T> m_Name = { nullptr };
        member_match = re.match(
            r'([\w:<>]+(?:\s*<[^>]+>)?)\s+(m_\w+)\s*(?:=\s*\{?\s*([^};]*?)\s*\}?)?\s*;',
            stripped
        )
        if member_match:
            type_str = member_match.group(1).strip()
            name_str = member_match.group(2).strip()
            default_str = member_match.group(3)
            if default_str is not None:
                default_str = default_str.strip()
            else:
                default_str = ""

            members.append({
                "name": name_str,
                "type": type_str,
                "default": default_str,
                "access": current_access
            })

    return members


def process_header(header_path: Path) -> Optional[Dict]:
    """단일 헤더 파일을 파싱하여 클래스 정보 딕셔너리 반환"""
    try:
        content = header_path.read_text(encoding='utf-8')
    except Exception as e:
        print(f"  [ERROR] Failed to read {header_path.name}: {e}")
        return None

    # _Define.h, _Function.h 등 유틸 헤더 건너뛰기
    if re.search(r'_Define\.h|_Function\.h', header_path.name):
        return None

    # class 키워드가 없으면 건너뛰기
    if not re.search(r'\bclass\s+', content):
        return None

    # namespace 감지
    ns = detect_namespace(content)

    # 클래스 추출
    class_match = re.search(
        r'\bclass\s+(?:ENGINE_DLL\s+)?(\w+)\s*(?:final|abstract)?\s*(?::([^{]*))?{',
        content,
        re.DOTALL
    )
    if not class_match:
        return None

    class_name = class_match.group(1)
    inheritance_str = class_match.group(2) or ""

    # 베이스 클래스
    base_class = extract_base_class(inheritance_str) if inheritance_str else None

    # Create / Clone 감지
    has_create = bool(re.search(r'\bstatic\s+.*\bCreate\s*\(', content))
    has_clone = bool(re.search(r'\bClone\s*\(', content))

    # Prefab 대상: Create가 있는 클래스만
    if not has_create:
        return None

    # 멤버 변수 추출
    members = extract_members(content, class_match.start())

    info = {
        "namespace": ns or "",
        "base_class": base_class or "",
        "has_create": has_create,
        "has_clone": has_clone,
        "members": members
    }

    print(f"  [OK] {class_name} ({len(members)} members)")
    return (class_name, info)


def main():
    parser = argparse.ArgumentParser(description='Header Parser for Prefab Metadata')
    parser.add_argument('input_dir', help='Input directory containing header files')
    parser.add_argument('output_json', help='Output JSON file path')

    args = parser.parse_args()

    input_dir = Path(args.input_dir)
    output_json = Path(args.output_json)

    print_header()
    print(f"  [INFO] Input:  {input_dir}")
    print(f"  [INFO] Output: {output_json}")
    print()

    if not input_dir.exists():
        print(f"  [ERROR] Input directory not found: {input_dir}")
        sys.exit(1)

    output_json.parent.mkdir(parents=True, exist_ok=True)

    classes = {}

    for header_file in sorted(input_dir.glob("*.h")):
        result = process_header(header_file)
        if result:
            class_name, info = result
            classes[class_name] = info

    metadata = {
        "version": "1.0",
        "classes": classes
    }

    try:
        output_json.write_text(
            json.dumps(metadata, indent=2, ensure_ascii=False),
            encoding='utf-8'
        )
    except Exception as e:
        print(f"  [ERROR] Failed to write {output_json}: {e}")
        sys.exit(1)

    print()
    print(f"  [RESULT] {len(classes)} classes exported to {output_json.name}")
    print("=" * 40)


if __name__ == "__main__":
    main()
