#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RTTR Registration Code Generator (Python Version)
Clone과 static Create 메서드를 자동으로 감지하여 RTTR 등록 코드 생성
"""

import sys
import os
import re
import argparse
from pathlib import Path
from typing import Set


def print_header():
    """헤더 출력"""
    print("=" * 40)
    print("RTTR Registration Code Generator")
    print("=" * 40)
    print()


def generate_rttr_code(class_name: str, has_clone: bool, has_create: bool) -> str:
    """RTTR 등록 코드 생성"""
    method_registrations = ""
    
    if has_clone:
        method_registrations += '\n        .method("Clone", &{}::Clone)'.format(class_name)
    
    if has_create:
        method_registrations += '\n        .method("Create", &{}::Create)'.format(class_name)
    
    rttr_code = f'''#include "{class_name}.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION
{{
    registration::class_<{class_name}>(L"{class_name}")
        .constructor<>(){method_registrations}
        ;
}}

'''
    return rttr_code


def process_header_file(header_path: Path, output_dir: Path, processed_classes: Set[str]) -> bool:
    """헤더 파일 처리"""
    try:
        content = header_path.read_text(encoding='utf-8')
    except Exception as e:
        print(f"===== [ERROR] Failed to read {header_path.name}: {e}")
        return False
    
    # enum이나 struct가 있으면 건너뛰기
    if re.search(r'\benum\s+', content) or re.search(r'\bstruct\s+', content):
        return False
    
    # class가 없으면 건너뛰기
    if not re.search(r'\bclass\s+', content):
        return False
    
    # 클래스 이름 추출
    class_match = re.search(r'\bclass\s+(?:ENGINE_DLL\s+)?(\w+)(?:\s+(?:final|abstract))?\s*[:{]', content)
    if not class_match:
        print(f"==========      [!WARN] {header_path.name} (class not found)")
        return False
    
    class_name = class_match.group(1)
    
    # 중복 체크
    if class_name in processed_classes:
        return False
    
    processed_classes.add(class_name)
    
    # Clone 메서드 존재 여부 확인
    has_clone = bool(re.search(r'\bClone\s*\(', content))
    
    # static Create 메서드 존재 여부 확인
    has_create = bool(re.search(r'\bstatic\s+.*\bCreate\s*\(', content))
    
    # RTTR 등록 코드 생성
    rttr_code = generate_rttr_code(class_name, has_clone, has_create)
    
    # 파일 저장
    output_file = output_dir / f"{class_name}_rttr.cpp"
    try:
        output_file.write_text(rttr_code, encoding='utf-8')
        print(f"==========      [OK] {class_name}_rttr.cpp")
        return True
    except Exception as e:
        print(f"===== [ERROR] Failed to write {output_file.name}: {e}")
        return False


def main():
    """메인 함수"""
    parser = argparse.ArgumentParser(description='RTTR Registration Code Generator')
    parser.add_argument('input_dir', help='Input directory containing header files')
    parser.add_argument('output_dir', help='Output directory for generated RTTR files')
    
    args = parser.parse_args()
    
    input_dir = Path(args.input_dir)
    output_dir = Path(args.output_dir)
    
    print_header()
    print(f"==========      [RTTR] Scanning directory: {input_dir}")
    print(f"==========      [RTTR] Output directory: {output_dir}")
    print()
    
    # 입력 디렉터리 확인
    if not input_dir.exists():
        print(f"=====       [ERROR] Input directory not found: {input_dir}")
        sys.exit(1)
    
    # 출력 디렉터리 생성
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # 처리된 클래스 추적
    processed_classes: Set[str] = set()
    count = 0
    
    # 모든 .h 파일 처리
    for header_file in sorted(input_dir.glob("*.h")):
        if process_header_file(header_file, output_dir, processed_classes):
            count += 1
    
    print()
    print(f"========== [RTTR] Total {count} files generated ==========")


if __name__ == "__main__":
    main()
