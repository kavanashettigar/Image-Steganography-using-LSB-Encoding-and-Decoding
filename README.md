# Image-Steganography-using-LSB-Encoding-and-Decoding
This project hides a secret text file within a BMP image by encoding the file size and content into the least significant bits (LSB) of image pixels.The image appears unchanged while carrying hidden data, enabling secure data transmission. During decoding, the hidden data is extracted and reconstructed back into the original text file.

# Features
- Hide secret text inside BMP image
- Extract hidden data from image
- Supports file size and content encoding
- Uses LSB (Least Significant Bit) technique
- Maintains original image quality

# Technologies Used
- Advanced C
- File Handling
- Pointers
- Bitwise Operations
- Functions
- Makefile
- Command Line Arguments

# How to Run
- Compile the program using: gcc *.c -o stego
- Run encoding: ./stego -e beautiful.bmp secret.txt output.bmp
- Run decoding: ./stego -d output.bmp decoded.txt
