# Flight Management System Encoder/Decoder

## Description
This project provides tools to encode and decode flight management system data. It can process CSV files, perform CRC checks, and handle different data formats.

## Features
- Encode and decode flight management system data.
- Parse CSV files.
- Perform CRC checks.
- Handle different data formats (e.g., one-meg and non-one-meg).

## Usage
### Compilation
To compile the project, open the Solution file `Encoder/Encoder.sln` in Visual Studio and build the solution. This will generate the encoder and decoder executables.

### Running
The encoder and decoder are run from the command line.

**Encoder:**
```bash
Encoder.exe <input_csv_file> <output_binary_file> [options]
```
- `<input_csv_file>`: Path to the input CSV file.
- `<output_binary_file>`: Path to save the encoded binary data.
- `[options]`: Optional arguments (e.g., for specifying data format). Refer to `Encoder.cpp` for details.

**Decoder:**
```bash
Decoder.exe <input_binary_file> <output_csv_file> [options]
```
- `<input_binary_file>`: Path to the input binary file.
- `<output_csv_file>`: Path to save the decoded CSV data.
- `[options]`: Optional arguments. Refer to `Encoder.cpp` for details.

## File Structure
- `Encoder/Encoder/Encoder.cpp`: Main source file for the encoder/decoder.
- `Encoder/Encoder/csv_parser.h` and `Encoder/Encoder/csv_parser.cpp`: CSV parser implementation.
- `Encoder/Encoder/onemeg_db.h` and `Encoder/Encoder/onemeg_db.cpp`: Functions related to one-meg data format.
- `Encoder/Encoder/nononemeg_db.h` and `Encoder/Encoder/nononemeg_db.cpp`: Functions related to non-one-meg data format.

## Contributing
Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch: `git checkout -b feature/your-feature-name`
3. Make your changes and commit them: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin feature/your-feature-name`
5. Create a new Pull Request.

## License
This project is licensed under the MIT License.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
