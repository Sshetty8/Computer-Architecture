#include <iostream>
#include <fstream>
#include <limits>
#include <bitset>
#include<sstream>

using byte = unsigned char ; 
using namespace std;
// number of bits in a byte (in practice, a byte is an octet ie. BITS_PER_BYTE == 8)

constexpr std::size_t BITS_PER_BYTE = 8;


// return the bits read from the file represented as a string of chars '0' or '1'
std::string read_bits( const std::string& file_name )
{   
    using bits = std::bitset<BITS_PER_BYTE> ; // bitset of bits in one byte
  
    if( std::ifstream file{ file_name, std::ios_base::hex } )
    {
        std::string result ;
        file >> std::noskipws ; // read every byte including white space

        byte b ;
	
	while( file >> b ) // for each byte read from the file
	{	result = bits(b).to_string() ; // append the bits in this byte to the result
		std::cout<<result<<std::endl;
	}              

	//std::cout<<result<<std::endl;
        return result ;
    }
    else return {} ; // read failure: return an empty string
}



// print the bits (default: to stdout) in a human readable form
std::ostream& print_bits( const std::string& bit_str, std::ostream& stm = std::cout )
{
    constexpr std::size_t BYTES_PER_LINE = 4 ;

    std::size_t nbits = 0 ;
    std::size_t nbytes = 0 ;
    for( char bit : bit_str ) // for each bit in the string
    {
        stm << bit ; // print the bit

        if( ++nbits%BITS_PER_BYTE == 0 ) // end of a byte
        {
            stm << ' ' ; // put a space after every byte

            // put a new line after every BYTES_PER_LINE bytes
            if( ++nbytes%BYTES_PER_LINE == 0 ) stm << '\n' ;
        }
    }

    return stm ;
}

int main()
{
    std::string str_bits = read_bits("trace1" ) ; // use this file for testing

    std::cout << "read " << str_bits.size() / BITS_PER_BYTE << " bytes ("
              << str_bits.size() << " bits)\n------------------------------\n" ;

    print_bits(str_bits);

    // edit the string of bits if required
    // call put_bits to write the modified bits into a file
}
