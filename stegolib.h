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
#define KEY_LEN		16
#endif

#ifndef IV_LEN
#define IV_LEN		12
#endif

#ifndef TAG_LEN
#define TAG_LEN		16
#endif
