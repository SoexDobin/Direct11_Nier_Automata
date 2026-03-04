#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Client RTTR Registration Code Generator (Single File Sandboxing)
오직 Client 프로젝트의 GameObject 및 Script 파생 객체만 감지하여
단일 파일(Client_RTTR.cpp)의 샌드박스 영역 내부에 안전하게 등록 코드를 주입합니다.
"""

import sys
import re
import argparse
from pathlib import Path
from typing import Set

def print_header():
    print("=" * 40)
    print("Client RTTR Registration Code Generator (Sandbox Mode)")
    print("=" * 40)
    print()

def process_header_file(header_path: Path, processed_classes: Set[str]) -> tuple[str, str]:
    """헤더 파일 분석 후 클래스 이름과 등록 코드 조각 반환"""
    try:
        content = header_path.read_text(encoding='utf-8')
    except Exception as e:
        print(f"===== [ERROR] Failed to read {header_path.name}: {e}")
        return None, None
        
    # RTTR_ENABLE 매크로가 없으면 무시
    if not re.search(r'\bRTTR_ENABLE\b', content):
        return None, None
        
    # GameObject 또는 Script(Component)를 상속/포함하는지 텍스트로 대략적으로 체크 (질문자 요청사항)
    is_target = re.search(r':\s*public\s+GameObject', content) or \
                re.search(r':\s*public\s+Script', content) or \
                re.search(r':\s*public\s+Component', content) or \
                re.search(r'\bRTTR_ENABLE\s*\(\s*(GameObject|Script|Component|.*?)\s*\)', content)
    
    if not is_target:
        return None, None

    # 클래스 이름 추출
    class_match = re.search(r'\bclass\s+(?:CLIENT_DLL\s+)?([A-Za-z0-9_]+)(?:\s+(?:final|abstract))?\s*[:{]', content)
    if not class_match:
        return None, None
        
    class_name = class_match.group(1)
    
    if class_name in processed_classes:
        return None, None
        
    processed_classes.add(class_name)
    
    has_clone = bool(re.search(r'\bClone\s*\(', content))
    has_create = bool(re.search(r'\bstatic\s+.*\bCreate\s*\(', content))
    
    # 등록 덩어리 생성
    method_registrations = ""
    if has_clone:
        method_registrations += '\n        .method("Clone", &{}::Clone)'.format(class_name)
    if has_create:
        method_registrations += '\n        .method("Create", &{}::Create)'.format(class_name)

    # ⭐ RTTR 문자열 칸에만 clean_name을 적용!
    rttr_block = f'''    rttr::registration::class_<{class_name}>(L"{clean_name}")
        .constructor<>()''' + method_registrations + ''';\n\n'''
    return class_name, rttr_block

def main():
    parser = argparse.ArgumentParser(description='Client RTTR Sandboxed Generator')
    parser.add_argument('input_dir', help='Input directory containing header files')
    parser.add_argument('output_file', help='Target output file (e.g., Client_RTTR.cpp) to sandbox')
    
    args = parser.parse_args()
    input_dir = Path(args.input_dir)
    output_file = Path(args.output_file)
    
    print_header()
    
    if not input_dir.exists():
        print(f"===== [ERROR] Input directory not found: {input_dir}")
        sys.exit(1)
        
    if not output_file.exists():
        print(f"===== [ERROR] Target File not found! Run the initial setup for {output_file.name}")
        sys.exit(1)
        
    processed_classes: Set[str] = set()
    new_includes = ""
    new_rttr_blocks = ""
    
    for header_file in sorted(input_dir.glob("*.h")):
        class_name, rttr_block = process_header_file(header_file, processed_classes)
        if class_name:
            new_includes += f'#include "{class_name}.h"\n'
            new_rttr_blocks += rttr_block
            print(f"==========      [Auto] Discovered target class: {class_name}")

    if not new_includes:
        print("==========      [INFO] No Target RTTR classes found.")
        return

    # 파일 읽기 및 정규식 교체 (샌드박싱)
    try:
        content = output_file.read_text(encoding='utf-8')
    except Exception as e:
        print(f"===== [ERROR] Could not read {output_file}: {e}")
        sys.exit(1)
        
    final_code = re.sub(r'(//\s*<AUTO_GENERATED_INCLUDES>\n).*?(\n//\s*</AUTO_GENERATED_INCLUDES>)',
                        r'\g<1>' + new_includes + r'\g<2>',
                        content, flags=re.DOTALL)
                        
    final_code = re.sub(r'(//\s*<AUTO_GENERATED_RTTR>\n).*?(\n//\s*</AUTO_GENERATED_RTTR>)',
                        r'\g<1>' + new_rttr_blocks + r'\g<2>',
                        final_code, flags=re.DOTALL)
                        
    try:
        output_file.write_text(final_code, encoding='utf-8')
        print(f"========== [SUCCESS] Sandboxed rewrite complete: {output_file.name}")
    except Exception as e:
        print(f"===== [ERROR] Failed to write changes: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
