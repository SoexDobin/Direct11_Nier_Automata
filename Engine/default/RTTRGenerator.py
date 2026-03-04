import sys
import re
import argparse
import os
from pathlib import Path
from typing import Set

def generate_rttr_logic(class_name: str, has_clone: bool, has_create: bool) -> str:
	method_registrations = ""
	if has_clone:
		method_registrations += f'\n\t\t.method("Clone", &{class_name}::Clone)'
	if has_create:
		method_registrations += f'\n\t\t.method("Create", &{class_name}::Create)'
	
	return f'\tregistration::class_<{class_name}>("{class_name}")\n\t\t.constructor<>()' + method_registrations + '\n\t\t;'

def get_full_file_content(class_name: str, rttr_logic: str, namespace: str) -> str:
	return f'''#include "pch.h"
#include "{class_name}.h"
#include <rttr/registration>

using namespace rttr;
using namespace {namespace};

RTTR_REGISTRATION
{{
{rttr_logic}
}}
'''

def process_header_file(header_path: Path, output_dir: Path, processed_classes: Set[str], namespace: str = "Engine") -> bool:
	try:
		content = header_path.read_text(encoding='utf-8')
		# Extract class name
		class_match = re.search(r'\bclass\s+(?:ENGINE_DLL\s+)?(\w+)', content)
		if not class_match:
			return False
		
		class_name = class_match.group(1)
		if class_name in processed_classes: return False
		processed_classes.add(class_name)

		# Skip enums and structs
		if re.search(r'\benum\s+', content) or re.search(r'\bstruct\s+', content):
			return False

		has_clone = bool(re.search(r'\bClone\s*\(', content))
		has_create = bool(re.search(r'\bstatic\s+.*\bCreate\s*\(', content))
		
		output_filename = f"{class_name}_rttr.cpp"
		output_file_path = output_dir / output_filename
		
		# [STRICT SKIP] If file exists, do not touch it at all.
		if output_file_path.exists():
			# Just print skip and return
			print(f"[Skip] {output_filename} already exists.")
			return False
		
		# Only generate if it doesn't exist
		new_logic = generate_rttr_logic(class_name, has_clone, has_create)
		full_content = get_full_file_content(class_name, new_logic, namespace)
		output_file_path.write_text(full_content, encoding='utf-8')
		print(f"[Create] {output_filename} generated.")
		return True

	except Exception as e:
		print(f"[Error] {header_path.name}: {e}")
		return False

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('input_dir', help='Header files path')
	parser.add_argument('output_dir', help='RTTR generated files path')
	parser.add_argument('--namespace', default='Engine')
	args = parser.parse_args()
	
	input_dir = Path(args.input_dir).resolve()
	output_dir = Path(args.output_dir).resolve()
	
	print(f"Scanning headers in: {input_dir}")
	print(f"Output directory: {output_dir}")
	print("-" * 50)
	
	if not input_dir.exists():
		print("Error: Input directory does not exist."); sys.exit(1)
	
	os.makedirs(output_dir, exist_ok=True)
	
	processed_classes = set()
	count = 0
	
	for header_file in sorted(input_dir.glob("*.h")):
		if process_header_file(header_file, output_dir, processed_classes, args.namespace):
			count += 1
			
	print("-" * 50)
	print(f"Done. Created {count} new files. Existing files were untouched.")

if __name__ == "__main__":
	main()
