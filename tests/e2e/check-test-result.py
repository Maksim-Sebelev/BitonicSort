#!/usr/bin/env python3

import sys
import subprocess

class Colors:
    WHITE = '\033[1;37m'
    GREEN = '\033[1;32m'
    RED = '\033[1;31m'
    RESET = '\033[0m'

def run_sort_program(executable, input_data):
    try:
        process = subprocess.run(
            [executable],
            input=input_data.encode('utf-8'),
            capture_output=True,
            text=False
        )
        stdout = process.stdout.decode('utf-8', errors='replace') if process.stdout else ""
        stderr = process.stderr.decode('utf-8', errors='replace') if process.stderr else ""
        return process.returncode, stdout, stderr
    except Exception as e:
        print(f"Error running program: {e}", file=sys.stderr)
        return None

def parse_numbers(text):
    """Extract numbers from text output."""
    numbers = []
    for token in text.split():
        token = token.strip(',[]()')
        if token:
            try:
                numbers.append(int(token))
            except ValueError:
                try:
                    numbers.append(float(token))
                except ValueError:
                    continue
    return numbers

def check_same_elements(original, result):
    """Check if result contains exactly the same elements as original."""
    if len(original) != len(result):
        return False, f"Length mismatch: expected {len(original)}, got {len(result)}"

    orig_sorted = sorted(original)
    res_sorted = sorted(result)
    
    for i in range(len(orig_sorted)):
        if orig_sorted[i] != res_sorted[i]:
            return False, f"Element mismatch at sorted position {i}: expected {orig_sorted[i]}, got {res_sorted[i]}"
    
    return True, ""

def viz_errs(numbers):
    if (len(numbers) <= 1):
        print(f"{Colors.GREEN}TEST PASSED{Colors.RESET}")
        return

    output = f"{Colors.GREEN}{numbers[0]}{Colors.RESET}"

    find_err = False

    for i in range(1, len(numbers)):
        if numbers[i - 1] <= numbers[i]:
            output += f" {Colors.GREEN}{numbers[i]}{Colors.RESET}"
        else:
            find_err = True
            output += f" {Colors.RED}{numbers[i]}{Colors.RESET}"
 
    if (find_err):
        print(output, end = '\n\n')
        print(f"{Colors.RED}TEST FAILED{Colors.RESET}")
        return 1

    else:
        print(f"{Colors.GREEN}TEST PASSED{Colors.RESET}")
        return 0

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <run-sort-exe> <n>.dat", file=sys.stderr)
        return 1
    
    executable = sys.argv[1]
    input_file = sys.argv[2]
    
    # Read input file
    try:
        with open(input_file, 'r') as f:
            input_data = f.read()
    except Exception as e:
        print(f"Error reading {input_file}: {e}", file=sys.stderr)
        return 1
    
    # Parse original numbers
    original_numbers = parse_numbers(input_data)
    if not original_numbers:
        print(f"Error: No numbers found in {input_file}", file=sys.stderr)
        return 1
    
    # Run sort program
    result = run_sort_program(executable, input_data)
    if result is None:
        print(f"{Colors.RED}TEST FAILED{Colors.RESET}")
        return 1
    
    exit_code, stdout, stderr = result
    
    # Check exit code
    if exit_code != 0:
        print("Program failed with exit code:", exit_code, file=sys.stderr)
        if stdout:
            print("STDOUT:", stdout, file=sys.stderr)
        if stderr:
            print("STDERR:", stderr, file=sys.stderr)
        
        print(f"{Colors.RED}TEST FAILED{Colors.RESET}")
        return 1

    # Parse result numbers
    result_numbers = parse_numbers(stdout)
    if not result_numbers:
        print("Error: Program produced no output", file=sys.stderr)
        return 1

    # Check if elements are the same
    same_elements, error_msg = check_same_elements(original_numbers, result_numbers)
    if not same_elements:
        print(f"Error: Output contains different elements than input\n{error_msg}", file=sys.stderr)
        print(f"{Colors.RED}TEST FAILED{Colors.RESET}")
        return 1

    return viz_errs(result_numbers)

if __name__ == "__main__":
    sys.exit(main())