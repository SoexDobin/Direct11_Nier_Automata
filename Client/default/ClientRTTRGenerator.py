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
from typing import Set, Tuple, Optional

def print_header():
    print("=" * 40)
    print("Client RTTR Registration Code Generator (Sandbox Mode)")
    print("=" * 40)
    print()

def process_header_file(header_path: Path, processed_classes: Set[str]) -> Tuple[Optional[str], Optional[str]]:
    """헤더 파일 분석 후 클래스 이름과 등록 코드 조각 반환"""
    try:
        content = header_path.read_text(encoding='utf-8')
    except Exception as e:
        print(f"===== [ERROR] Failed to read {header_path.name}: {e}")
        return None, None
        
    # RTTR_ENABLE과 static Create가 모두 있으면 타겟으로 간주 (상속 깊이 상관없음)
    has_rttr = bool(re.search(r'\bRTTR_ENABLE\b', content))
    has_create = bool(re.search(r'\bstatic\s+.*\bCreate\s*\(', content))
    
    if not (has_rttr and has_create):
        return None, None
 
    # 상속 관계를 분석하여 반환 타입(GameObject vs Component) 결정
    # Component 계열 감지 (부모 클래스 이름 또는 직접 상속 텍스트 확인)
    is_component = re.search(r':\s*public\s+(?:Script|Component|StateMachine)', content) or \
                   re.search(r'\bRTTR_ENABLE\s*\(\s*(?:Script|Component|StateMachine)\s*\)', content)
    
    return_type = "Shared<Component>" if is_component else "Shared<GameObject>"

    # 클래스 이름 추출
    class_match = re.search(r'\bclass\s+(?:CLIENT_DLL\s+)?([A-Za-z0-9_]+)(?:\s+(?:final|abstract))?\s*[:{]', content)
    if not class_match:
        return None, None
        
    class_name = class_match.group(1)
    
    if class_name in processed_classes:
        return None, None
        
    processed_classes.add(class_name)
    
    # 메타데이터 파싱
    level_match = re.search(r'//\s*\[RTTR_LEVEL\]\s*(LEVEL::[a-zA-Z0-9_]+|\d+)', content)
    target_level = level_match.group(1) if level_match else "0"

    has_clone = bool(re.search(r'\bClone\s*\(', content))
    has_create = bool(re.search(r'\bstatic\s+.*\bCreate\s*\(', content))
    
    if not has_create:
        return None, None
    
    # 등록 덩어리 생성
    method_registrations = ""
    if has_clone:
        method_registrations += '\n        .method("Clone", &{}::Clone)'.format(class_name)
    if has_create:
        method_registrations += f'''
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> {return_type} {{
            return {class_name}::Create(device, context);
        }})(rttr::metadata("Level", {target_level}))'''

    # ⭐ RTTR 문자열 칸에만 clean_name을 적용!
    rttr_block = f'''    rttr::registration::class_<{class_name}>("{class_name}")
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
    
    # Pre-scan output file for already manually registered classes
    if output_file.exists():
        try:
            existing_content = output_file.read_text(encoding='utf-8')
            manual_content = re.sub(r'(?s)//\s*<AUTO_GENERATED_RTTR>.*?//\s*</AUTO_GENERATED_RTTR>', '', existing_content)
            for match in re.finditer(r'rttr::registration::class_<([A-Za-z0-9_]+)>', manual_content):
                manual_class = match.group(1)
                processed_classes.add(manual_class)
                # print(f"==========      [Info] Found manual registration: {manual_class}")
        except Exception as e:
            print(f"==========      [Warning] Could not pre-scan manual registrations: {e}")
    
    for header_file in sorted(input_dir.glob("*.h")):
        class_name, rttr_block = process_header_file(header_file, processed_classes)
        if class_name and rttr_block:
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
        
    print(f"========== [DEBUG] Content length: {len(content)}")
    
    include_pattern = r'(//\s*<AUTO_GENERATED_INCLUDES>).*?(//\s*</AUTO_GENERATED_INCLUDES>)'
    if re.search(include_pattern, content, flags=re.DOTALL):
        print("========== [DEBUG] Found Includes Marker")
    else:
        print("========== [DEBUG] Includes Marker NOT FOUND")

    final_code = re.sub(include_pattern,
                        r'\g<1>\n' + new_includes + r'\g<2>',
                        content, flags=re.DOTALL)
                        
    rttr_pattern = r'(//\s*<AUTO_GENERATED_RTTR>).*?(//\s*</AUTO_GENERATED_RTTR>)'
    if re.search(rttr_pattern, final_code, flags=re.DOTALL):
        print("========== [DEBUG] Found RTTR Marker")
    else:
        print("========== [DEBUG] RTTR Marker NOT FOUND")

    final_code = re.sub(rttr_pattern,
                        r'\g<1>\n' + new_rttr_blocks + r'\g<2>',
                        final_code, flags=re.DOTALL)
                        
    try:
        # 모든 줄바꿈을 LF(\n)로 통일 (기존에 섞여있을 수 있는 \r\n 제거)
        # 그 후 open의 newline='\r\n' 옵션이 모든 \n을 \r\n으로 변환함
        final_code = final_code.replace('\r\n', '\n')
        with open(output_file, "w", encoding='utf-8', newline='\r\n') as f:
            f.write(final_code)
        print(f"========== [SUCCESS] Sandboxed rewrite complete: {output_file.name}")
    except Exception as e:
        print(f"===== [ERROR] Failed to write changes: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
