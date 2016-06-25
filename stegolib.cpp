#include <fstream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stack>
#include <podofo/podofo.h>

#include "3rdparty/base64.h"
#include "stegolib.h"

using namespace std;
using namespace PoDoFo;

/*
 * Encrypt the provided plaintext with the provided password
 */
void enc(char* plaintxt_fn, char* ciphertxt_fn, char* pw) {
    int ret;
    int outlen;
    int plaintxtlen;
    ifstream infile;
    ofstream outfile;
    streampos filesize;
    unsigned char iv[IV_LEN];
    unsigned char tag[TAG_LEN];
    unsigned char* key;
    unsigned char* plaintxt_data;
    unsigned char* ciphertxt_data;
    unsigned char* outbuf;

    /* read plaintxt data from file */
    infile.open(plaintxt_fn, ios::in|ios::binary|ios::ate);
    filesize = infile.tellg();
    plaintxt_data = new unsigned char [filesize];
    infile.seekg(0, ios::beg);
    infile.read((char*)plaintxt_data, filesize);
    infile.close();
    plaintxtlen = filesize;

    /* initialize cipher output and outbuf */
    ciphertxt_data = new unsigned char [filesize];
    outbuf = new unsigned char [filesize];

    /* generate keyhash */
    key = (unsigned char *) malloc(sizeof(unsigned char) * KEY_LEN);
    hashpassword(pw, key, KEY_LEN);
    if (key == NULL) {
        printf("Failed to generate crypto hash password\n");
        return;
    }

    /* generate a new, random IV */
    if (!RAND_bytes(iv, sizeof(iv))) {
        /* OpenSSL reports a failure, act accordingly */
        printf("Failed to generate a random IV\n");
        return;
    }

    /* create cipher objects */
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    /* initialize encryption type (AES-128 GCM) */
    ret = EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    assert(ret == 1);
    /* set IV Len */
    ret = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), NULL);
    assert(ret == 1);
    /* initialize key and IV */
    ret = EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);
    assert(ret == 1);
    /* encrypt the plaintext*/
    ret = EVP_EncryptUpdate(ctx, outbuf, &outlen, plaintxt_data, plaintxtlen);
    assert(ret == 1);
    /* finalise */
    ret = EVP_EncryptFinal_ex(ctx, outbuf, &outlen);
    assert(ret == 1);
    /* copy outbuf to ciphertxt_data */
    memcpy(ciphertxt_data, outbuf, plaintxtlen);
    /* get the tag*/
    ret = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, outbuf);
    assert(ret == 1);
    /* copy outbuf to tag_data */
    memcpy(tag, outbuf, TAG_LEN);
    /* free */
    EVP_CIPHER_CTX_free(ctx);

    /* write iv and ciphertext to file */
    outfile.open(ciphertxt_fn);
    outfile.write((const char*)iv, sizeof(iv));
    outfile.write((const char*)tag, TAG_LEN);
    outfile.write((const char*)ciphertxt_data, plaintxtlen);
    outfile.close();

    /* Print info message */
    printf("Successfully encrypted %d bytes of data\n", plaintxtlen);
}

/*
 * Decrpyt the cipher file with provided password
 */
void dec(char* cipher_fn, char* output_fn, char* pw) {
    int outlen;
    int ret;
    int cipherlen;
    ifstream infile;
    ofstream outfile;
    streampos filesize;
    unsigned char* key;
    unsigned char* filedata;
    unsigned char* ciphertxt;
    unsigned char* outbuf;
    unsigned char iv[IV_LEN];
    unsigned char tag[TAG_LEN];
    EVP_CIPHER_CTX *ctx;

    /* read raw data from file */
    infile.open(cipher_fn, ios::in|ios::binary|ios::ate);
    filesize = infile.tellg();
    filedata = new unsigned char [filesize];
    infile.seekg(0, ios::beg);
    infile.read((char*)filedata, filesize);
    infile.close();

    /* format: [ (16b iv) (16b tag) (rest is cipher...) ] */
    cipherlen = (int)filesize-sizeof(iv)-sizeof(tag);
    ciphertxt = new unsigned char [cipherlen];
    outbuf = new unsigned char [filesize];
    memcpy(iv, filedata, sizeof(iv));
    memcpy(tag, filedata+sizeof(iv), sizeof(tag));
    memcpy(ciphertxt, filedata+sizeof(iv)+sizeof(tag), cipherlen);

    /* build key hash */
    key = (unsigned char *) malloc(sizeof(unsigned char) * KEY_LEN);
    hashpassword(pw, key, KEY_LEN);
    if (key == NULL) {
        printf("Failed to generate crypto hash password\n");
        return;
    }

    /* create cipher objects */
    ctx = EVP_CIPHER_CTX_new();
    /* setup encryption type (AES-128 GCM) */
    ret = EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    assert(ret == 1);
    /* set IV length */
    ret = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), NULL);
    assert(ret == 1);
    /* initialize with enc key + iv */
    ret = EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv);
    assert(ret == 1);
    /* input ciphertxt */
    ret = EVP_DecryptUpdate(ctx, outbuf, &outlen, ciphertxt, cipherlen);
    assert(ret == 1);
    /* set expected tag */
    ret = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, sizeof(tag), tag);
    /* Finalise */
    ret = EVP_DecryptFinal_ex(ctx, outbuf, &outlen);

    /* check tag */
    if (ret <= 0) {
        printf("Tag verify has failed!!\n");
        return;
    }

    /* free */
    EVP_CIPHER_CTX_free(ctx);

    /* write decrypted plaintext to file */
    outfile.open(output_fn);
    outfile.write((const char*)outbuf, cipherlen);
    outfile.close();

    /* Print info message */
    printf("Decrypted %d bytes of data\n", cipherlen);
}

/*
 * Hash the provided password with PKCS5_PBKDF2_HMAC_SHA1
 */
void hashpassword(char* pw, unsigned char* digest, int digestlen) {
    long iterations = 10000;
    unsigned char salt[] = {'b','a','r','f'};
    PKCS5_PBKDF2_HMAC_SHA1(pw, strlen(pw), salt, sizeof(salt), iterations, digestlen, digest);
}

//////////////////////////////////////////////////////
//***************** PDF Functions *****************//
////////////////////////////////////////////////////

/*
 * Embed the provided message file into the provided cover pdf.
 * Returns: number of bytes embedded (if successful, entire message size)
 * 
 * Or ff in counting mode, returns maximum bits available to 
 * embed the message in.
 */
void emb_pdf(char* cover_fn, char* message_fn, char* stego_fn) {
    ifstream messagef;
    streampos filesize;
    unsigned char* messagedata;

    /* read contents of message file into memory */
    messagef.open(message_fn, ios::in|ios::binary|ios::ate);
    filesize = messagef.tellg();
    messagedata = new unsigned char [filesize];
    messagef.seekg(0, ios::beg);
    messagef.read((char*)messagedata, filesize);
    messagef.close();

    /* encode the message in base64 */
    int b64asciilen;
    char* b64msg = base64(messagedata, filesize, &b64asciilen);

    /* open cover pdf file */
    PdfMemDocument pdf(cover_fn);

    /* get last page */
    PdfPage* pPage = pdf.GetPage(pdf.GetPageCount()-1);

    /* Use PdfPainter to write our message */
    PdfPainter painter;
    painter.SetPage(pPage);
    PdfFont* pFont = pdf.CreateFont("Arial");
    pFont->SetFontSize(0.0);
    painter.SetFont(pFont);
    painter.SetColor(0,0,0);
    painter.DrawText(0,0, b64msg);
    painter.FinishPage();

    /* write stego file */
    pdf.Write(stego_fn);
}

/*
 * Extract the hidden message from the provided stego pdf file
 */
void ext_pdf(char* stego_fn, char* extract_fn) {
    const char*      pszToken = NULL;
    PdfVariant       var;
    EPdfContentsType eType;
    double dCurPosX     = 0.0;
    double dCurPosY     = 0.0;
    double dCurFontSize = 0.0;
    PdfFont* pCurFont   = NULL;
    ofstream outfile;

    /* open the stego pdf file */
    PdfMemDocument pdf(stego_fn);

    /* stego is in last page */
    PdfPage* pPage = pdf.GetPage(pdf.GetPageCount()-1);

    /* iterate through PDF operators */
    PdfContentsTokenizer tokenizer( pPage );
    stack<PdfVariant> stack;
    while (tokenizer.ReadNext(eType, pszToken, var)) {
        if (eType == ePdfContentsType_Keyword) {
            /* 'l' and 'm' tokens tell us x/y coord for string */
            if (strcmp( pszToken, "l" ) == 0 || 
                strcmp( pszToken, "m" ) == 0) {
                dCurPosX = stack.top().GetReal();
                stack.pop();
                dCurPosY = stack.top().GetReal();
                stack.pop();
            }

            /* 'Tf' contains the font and font size*/
            if (strcmp( pszToken, "Tf" ) == 0) {
                dCurFontSize = stack.top().GetReal();
                stack.pop();
                PdfName fontName = stack.top().GetName();
                PdfObject* pFont = pPage->GetFromResources( PdfName("Font"), fontName );
                pCurFont = pdf.GetFont(pFont);
            }

            /* Tj and ' contain the string */
            else if (strcmp( pszToken, "Tj" ) == 0 ||
                     strcmp( pszToken, "'" ) == 0) {
                if (dCurPosX == 0 && dCurPosY == 0 && dCurFontSize == 0) {
                    /* hidden message found! convert to utf8 cstr */
                    PdfString pstr = stack.top().GetString();
                    stack.pop();
                    PdfString unicode = pCurFont->GetEncoding()->ConvertToUnicode(pstr, pCurFont);
                    const char* b64msg = unicode.GetStringUtf8().c_str();

                    /* decode base64 message */
                    int datalen;
                    unsigned char* decodedmsg = unbase64(b64msg, strlen(b64msg), &datalen);

                    /* write decode message to file */
                    outfile.open(extract_fn);
                    outfile.write((const char*)decodedmsg, datalen);
                    outfile.close();
                }
            }
        } else if (eType == ePdfContentsType_Variant) {
            stack.push(var);
        } else {
            printf("Unkown type parsed in pdf file\n");
        }
    }
}

/*
 * Encrypt & embed the provided message file into the cover pdf file
 */
void encemb_pdf(char* cover_fname, char* message_fn, char* stego_fname, char* pw) {
    char *tmp_encrypted;
    tmp_encrypted = (char*)"encrypted.raw";
    enc(message_fn, tmp_encrypted, pw);
    emb_pdf(cover_fname, tmp_encrypted, stego_fname);
    if ( remove(tmp_encrypted) != 0 ) {
        printf("Failed to remove temporary encrypted file\n");
    }
}

/*
 * Extract & decrypt the provided stego file
 */
void extdec_pdf(char* stego_fn, char* message_fn, char* pw) {
    char *tmp_encrypted;
    tmp_encrypted = (char*)"encrypted.raw";
    ext_pdf(stego_fn, tmp_encrypted);
    dec(tmp_encrypted, message_fn, pw);
    if ( remove(tmp_encrypted) != 0 ) {
        printf("Failed to remove temporary encrypted file\n");
    }
}
