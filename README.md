# Steganography

### Overview

Implementation of LSB [steganography][wiki]. Hide text, files and even other images into images.

### Example

#### Original image.
![alt text][rome]

#### Modified image
![alt text][steg]

While the two images appear identical, the second image has the entire works of William Shakespeare embedded into the least significant bits.

### Notes
Least significant bit steganography is by no means the state of the art and this was mostly done as a thought experiment. JPEG steganography is both more robust and much harder to detect. A well known implementation of this can be found [here][jsteg].

#### Limitations
There are many known limitations of this implementation.
* This cannot embed data into jpegs. Only lossless data formats will work.
* Large images are required to embed data. You can use more bits per pixel to embed more data but this will begin to introduce visible artifacts at above 2LSB.

[wiki]: https://en.wikipedia.org/wiki/Steganography
[rome]: https://upload.wikimedia.org/wikipedia/commons/5/53/Colosseum_in_Rome%2C_Italy_-_April_2007.jpg
[steg]: https://github.com/maxwell-yaron/steganography/raw/master/steg.png
[jsteg]: https://github.com/lukechampine/jsteg
