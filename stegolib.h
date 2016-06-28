/*
 * PDFStego, Copyright (C) 2016  Aaron Gallagher
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Contact: Aaron Gallagher <aaron.b.gallagher@gmail.com>
 */


/* Encryption/Decryption */
void hashpassword(char* pw, unsigned char* digest, int digestlen);
void enc(char* plaintxt_fn, char* ciphertxt_fn, char* pw);
void dec(char* cipher_fn, char* output_fn, char* pw);

/* Embed/Extract data from pdf file */
void emb_pdf(char* cover_fn, char* message_fn, char* stego_fn);
void ext_pdf(char* stego_fn, char* extract_fn);

/* Complete stego operations */
void encemb_pdf(char* cover_fname, char* message_fname, char* stego_fname, char* pw);
void extdec_pdf(char* stego_fn, char* message_fn, char* pw);

/* Encryption Consts */
#ifndef KEY_LEN
#define KEY_LEN     16
#endif

#ifndef IV_LEN
#define IV_LEN      12
#endif

#ifndef TAG_LEN
#define TAG_LEN     16
#endif
