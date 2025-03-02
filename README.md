# p3_parser

## Overview

`p3_parser` is a bottom-up parser for arithmetic expressions. It reads an input file containing a sequence of tokens and produces a debug output file that traces the parsing steps.

## Usage

To run the parser, use the following command:

```sh
parser <input_file>
```

- `<input_file>`: Path to the input file containing the token sequence (e.g., `tests/test_input1.cscn`).

The output will be saved to a file with the same base name as the input file, but with `_p3dbg.txt` appended. For example, if the input file is `test_input1.cscn`, the output will be saved to `test_input1_p3dbg.txt`.

> [!NOTE]
> If your using MacOS/ Linux the previouse commnad may not work, try this command instead:
>```sh
>./parser <input_file>
>```


## Input File Format

The input file should contain a sequence of tokens in the following format:

```
<2, NUM> <+, PLUS> <3, NUM> <*, STAR> <(, LPAREN> <4, NUM> <+, PLUS> <5, NUM> <), RPAREN>
```

Each token is represented by a pair of values: the lexeme and the token type.

## Output File Format

The output file contains a detailed trace of the parsing steps, including the current state, stack contents, input position, operation, and action taken. Here is an example of the output format:

```
Step 1:
Current State: 0
Stack Contents: [[0 <EOF, "$", 0, 0>]]
Input Position: <NUM, "2", 1, 0>
Operation: SHIFT
Action: Shift to state 5

Step 2:
Current State: 5
Stack Contents: [[0 <EOF, "$", 0, 0>] [5 <NUM, "2", 1, 0>]]
Input Position: <PLUS, "+", 1, 9>
Operation: REDUCE
Action: Reduce by rule 7: f → NUM
```

## Compilation

To compile the parser, use the provided `Makefile`. Run the following command in the terminal:

```sh
make
```

This will compile the source files and produce the `parser` executable.

## Cleaning Up

To remove the compiled files and the output files, run:

```sh
make clean
```

## Running Tests

To run the parser with a sample input file, use the following command:

```sh
make test
```

This will run the parser with the `tests/test_input1.cscn` file.

To run the parser with a custom input file, use:

```sh
make run INPUT=<file>
```

Replace `<file>` with the path to your input file.

> [!CAUTION]
> Ensure you compile the parser first before attempting these two test options.


## Tests

### Test Case 1: 
test_input1.cscn



**Input:**
```plaintext
<2, NUM> <+, PLUS> <3, NUM> <*, STAR> <(, LPAREN> <4, NUM> <+, PLUS> <5, NUM> <), RPAREN>
```

**Explanation:**
This input represents the arithmetic expression `2 + 3 * (4 + 5)`. The tokens are provided in the format `<lexeme, token_type>`.

**Expected Output:**
The output file will contain a detailed trace of the parsing steps, including the current state, stack contents, input position, operation, and action taken. The parser will perform a series of SHIFT and REDUCE operations to parse the expression according to the grammar rules.

### Test Case 2: 

test_input2.cscn



**Input:**
```plaintext
(5 + 2) * 3 + 7 * (4 + 1)
```

**Explanation:**
This input represents the arithmetic expression `(5 + 2) * 3 + 7 * (4 + 1)`. The tokens are provided as a plain arithmetic expression.

**Expected Output:**
The output file will contain a detailed trace of the parsing steps. The parser will tokenize the input expression and then perform a series of SHIFT and REDUCE operations to parse the expression according to the grammar rules.

### Test Case 3: 

test_input3.cscn



**Input:**
```plaintext
<7, NUM> <+, PLUS> <*, STAR> <5, NUM> <), RPAREN>
```

**Explanation:**
This input represents an incomplete or malformed arithmetic expression `7 + * 5 )`. The tokens are provided in the format `<lexeme, token_type>`.

**Expected Output:**
The output file will contain a detailed trace of the parsing steps. The parser will likely encounter an error due to the malformed expression and will not be able to successfully parse it. The output will include the point at which the error occurred.

### Test Case 4: 

test_input4.csn



**Input:**
```plaintext
<2, NUM> <+, PLUS> <3, NUM> <*, STAR> <(, LPAREN> <4, NUM> <+, PLUS> <5, NUM> <), RPAREN>
```

**Explanation:**
This input is identical to 

test_input1.cscn

 and represents the arithmetic expression `2 + 3 * (4 + 5)`. The tokens are provided in the format `<lexeme, token_type>`.

**Expected Output:**
The output file will contain a detailed trace of the parsing steps, similar to 

test_input1.cscn

. The parser will perform a series of SHIFT and REDUCE operations to parse the expression according to the grammar rules.

### Example Output Format

Here is an example of what the output might look like for a successful parsing operation:

```plaintext
Step 1:
Current State: 0
Stack Contents: [0 $]
Input Position: 2 + 3 * (4 + 5)
Operation: SHIFT
Action: Shift to state 5

Step 2:
Current State: 5
Stack Contents: [0 $] [5 2]
Input Position: + 3 * (4 + 5)
Operation: REDUCE
Action: Reduce by rule 7: f → NUM

Step 3:
Current State: 3
Stack Contents: [0 $] [3 f]
Input Position: + 3 * (4 + 5)
Operation: REDUCE
Action: Reduce by rule 5: t → f
```

This output shows the detailed steps the parser takes to process the input expression, including the state transitions and actions performed.

## License
This project is licensed under the ***Pompeu Fabra University*** License.