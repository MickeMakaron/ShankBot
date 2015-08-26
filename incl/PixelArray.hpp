#ifndef PIXELARRAY_H
#define PIXELARRAY_H


/// \brief Container for pixel data
/// The PixelArray object assumes ownership of the pixel data
/// given in the constructor parameters. This means memory of
/// pixel data will be deallocated at object destruction.
class PixelArray
{
    public:
        /// \brief Constructor
        ///
        /// \param pixels char* Pixel data
        /// \param size unsigned int Size of pixel data
        ///
        ///
        PixelArray(char* pixels, unsigned int size);

        /// \brief Destructor
        ///
        ///
        ///
        ~PixelArray();


        /// \brief Access operator
        ///
        /// \param i unsigned int Index of pixel
        /// \return const char& Pixel at given index
        ///
        ///
        const char& operator[](unsigned int i) const;

        /// \brief Get pixel C-array
        ///
        /// \return char* Pointer to pixel C-array
        ///
        ///
        char*  getPixels();


        /// \brief Get size of pixel array.
        ///
        /// \return unsigned int Size of pixel array.
        ///
        ///
        unsigned int getSize() const;

    private:
        char*           mPixels; ///< Pixel values
        unsigned int    mSize;   ///< Size of pixel array
};

#endif // PIXELARRAY_H
