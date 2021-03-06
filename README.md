# PDFStego

[![Build Status](https://travis-ci.org/aagallag/PDFStego.svg?branch=master)](https://travis-ci.org/aagallag/PDFStego)

PDFStego is a tool that allows you to embed a hidden message in an innocent pdf file.  Before embedding the hidden message, the application will first encrypt the message data with AES-128 GCM and then encode the encrypted data as base64.  After embedding the encoded/encrpyted secret data in the pdf file, the pdf file will appear completely unchanged to the average, uninformed computer user.

## Warning
While most PDF viewers won't render the hidden text, in the application's current state it's very easy to detect the presence of steganographic content.  A tool for extracting text from pdf files is likely to expose the encrypted version of the secret message.  If a secure password is used, the contents of your hidden messsage should still be safe as this will only expose the encrypted version of your message.

For example, if you have PoDoFo installed, the stego content can be detected with

```
$ podofotxtextract stego.pdf
```

For a more secure PDF steganography tool, I recommend checking out [pdf_hide](https://github.com/ncanceill/pdf_hide).

## Environment Setup

Install the PoDoFo library

```bash
$ sudo apt-get install libpodofo-dev
```

## PDFStego Usage

```bash
$ ./pdfstego -h
PDFStego, Copyright (C) 2016  Aaron Gallagher
This is free software, and you are welcome to redistribute it

Usage:

  ::Embed::
	./pdfstego -e -c <coverfile> -m <messagefile> -s <stegofile>
  ::Extract::
	./pdfstego -x -s <stegofile> -m <messagefile>
```

## 3rd Party License Info

Base64 encoding/decoding is performed with base64.h from the project [NibbleAndAHalf](https://github.com/superwills/NibbleAndAHalf) by William Sherif.

PDF parsing and editing is performed with the PoDoFo library.

## PDFStego License

```
PDFStego, Copyright (C) 2016  Aaron Gallagher

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact: Aaron Gallagher <aaron.b.gallagher@gmail.com>
```
