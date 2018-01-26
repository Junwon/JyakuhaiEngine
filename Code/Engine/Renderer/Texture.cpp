#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Texture::Texture() : 
	m_imageFilePath( "UNKNOWN IMAGE FILE" ), 
	m_textureID( 0 )
{
}


Texture::Texture(const std::string& imageFilePath, unsigned int textureID, int width, int length) :
	m_imageFilePath( imageFilePath ),
	m_textureID( textureID ),
	m_texelDimensions(width, length)
{
}

//-----------------------------------------------------------------------------------------------
Texture::~Texture()
{

}

