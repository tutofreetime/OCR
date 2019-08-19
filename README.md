# OCR

Optical character recognition software written in C.

## Requirements

Debian : 
- gcc
````
sudo apt install gcc 
````
- GTK3
````
sudo apt-get install libgtk-3-dev
````
- SDL2
````
sudo apt install libsdl2-dev
````
- SDL2_image : 
````
sudo apt install libsdl2-image-dev
````

If you plan to train the network you will also need:

- python
- xelatex
- pdftoppm

## Build

```bash
make
```

## Usage

```bash
./ocr
```

## Training

1. Generate the custom dataset used to train the neural network
    ```bash
    cd dataset
    # This can take a while
    ./generate_dataset.sh
    ```
2. (optional) Adjust training parameters in `src/ocr_train.c`
3. Use the `--train` option when launching the OCR
    ```bash
    ./ocr --train
    ```

This will output the neural network in `output/ocr_network_eX` after each epoch.

*You can save the pre-computed dataset to avoid wasting time before each
training (see details about this in `src/ocr_train.c`).*
