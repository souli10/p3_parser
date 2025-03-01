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

> [!WARNING]
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

## Additional Information

For more details on the implementation and the grammar rules used by the parser, refer to the source code and the comments within the files.

## License
This project is licensed under the ***Pompeu Fabra University*** License.