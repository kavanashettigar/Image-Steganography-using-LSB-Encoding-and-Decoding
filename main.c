/*
Name : Kavana
Project Name : Stegnography
Project Description : Steganography is a technique of securely hiding secret information inside a multimedia file such as 
an image without causing any noticeable change. In this project, LSB (Least Significant Bit) image steganography is 
implemented using a BMP image to conceal a secret text file.
Encoding:
The encoding process hides secret data inside the source BMP image by modifying the least significant bits of the image 
pixels. The file size, extension, and content are stored one by one, and the final stego image looks the same as the 
original image.
Decoding:
The decoding process extracts the hidden information by reading the LSBs of the stego image in the same order used during 
encoding. The secret file is reconstructed by retrieving its extension, size, and original content from the embedded data.

*/
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;// Structure to store encoding-related information
    DecodeInfo decInfo;// Structure to store decoding-related information
    if(argc<=2)// Check for insufficient arguments during decoding
    {
        printf("ERROR: Invalid arguments\n");
        printf("Usage-For encoding : ./a.out -e [src.bmp] [secret.txt] <dest.bmp>\n");
        printf("For decoding : ./a.out -d [dest.bmp]  <output>\n");
        return 0;
    }
    if((strcmp(argv[1],"-e")==0)&& argc<4)// Check for insufficient arguments during encoding
    {
        printf("ERROR: Invalid arguments\n");
        printf("Usage-For encoding : ./a.out -e [src.bmp] [secret.txt] <dest.bmp>\n");
        printf("For decoding : ./a.out -d [dest.bmp]  <output>\n");
        return 0;
    }
    
    // checking operation type (encode/decode/unsupported)
    if (check_operation_type(argv[1]) == e_unsupported)
    {
        printf("ERROR: Unsupported operation\n");
        return 0;
    }
    else if(check_operation_type(argv[1]) == e_encode)
    {
        if(read_and_validate_encode_args(argv,&encInfo)==e_failure)// Validate encoding arguments
        {
            printf("ERROR: Invalid extensions\n");
            return 0;
        }
        do_encoding(&encInfo);// Perform encoding

    }
    else
    {
        if(read_and_validate_decode_args(argv,&decInfo)==e_failure)// Validate decoding arguments
        {
           printf("ERROR: Invalid extensions\n");
           return 0; 
        }
        do_decoding(&decInfo);// Perform decoding
    }
}
OperationType check_operation_type(char *symbol)
{
    if(strcmp(symbol,"-e")==0)// Check if encoding option is selected
    return e_encode;
    else if(strcmp(symbol,"-d")==0)// Check if decoding option is selected
    return  e_decode;
    else
    return e_unsupported;// Unsupported operation

}
