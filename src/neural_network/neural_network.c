#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/math.h"
#include "../utils/random.h"
#include "neural_network.h"
size_t returnError = 0;

static void layer_alloc(struct Layer *layer, size_t nb_in, size_t nb_out)
{
    layer->nb_neurons = nb_out;
    layer->in = matrix_alloc(nb_in, 1);
    layer->out = matrix_alloc(nb_out, 1);
    layer->weight = matrix_alloc(nb_out, nb_in);
    layer->bias = matrix_alloc(nb_out, 1);
    layer->delta = matrix_alloc(nb_out, 1);

    // Init weight and bias with normal distribution (truncated for weight)
    double sqrt_n = sqrt(layer->weight->nb_cols);
    for (size_t i = 0; i < matrix_size(layer->weight); i++)
        layer->weight->mat[i] = normal_distribution(0., 1.) / sqrt_n;
    for (size_t i = 0; i < matrix_size(layer->bias); i++)
        layer->bias->mat[i] = normal_distribution(0., 1.);
}

static void layer_free(struct Layer *layer)
{
    matrix_free(layer->in);
    matrix_free(layer->out);
    matrix_free(layer->weight);
    matrix_free(layer->bias);
    matrix_free(layer->delta);
}

struct Network *network_alloc(size_t nb_layers, size_t *layers_size)
{
    struct Network *network = malloc(sizeof(struct Network));
    network->nb_layers = nb_layers;
    network->layers = malloc(sizeof(struct Layer) * nb_layers);
    for (size_t i = 0; i < nb_layers; i++)
    {
        struct Layer *layer = &network->layers[i];
        size_t prev_layer_size = 0;
        if (i > 0)
            prev_layer_size = layers_size[i - 1];
        layer_alloc(layer, prev_layer_size, layers_size[i]);
    }
    return network;
}

void network_free(struct Network *network)
{
    for (size_t i = 0; i < network->nb_layers; i++)
        layer_free(&network->layers[i]);
    free(network->layers);
    free(network);
}

void network_save(struct Network *network, const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
        errx(1, "cannot save network to %s", filename);

    fwrite(&network->nb_layers, sizeof(size_t), 1, f);
    for (size_t i = 0; i < network->nb_layers; i++)
        fwrite(&network->layers[i].nb_neurons, sizeof(size_t), 1, f);

    for (size_t i = 0; i < network->nb_layers; i++)
    {
        struct Layer *layer = &network->layers[i];
        matrix_save(f, layer->weight);
        matrix_save(f, layer->bias);
    }

    fclose(f);
}

struct Network *network_load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
        errx(1, "cannot load network from %s", filename);

    size_t nb_layers = 0;
    returnError = fread(&nb_layers, sizeof(size_t), 1, f);
    size_t *layers_size = malloc(sizeof(size_t) * nb_layers);
    returnError = fread(layers_size, sizeof(size_t) * nb_layers, 1, f);

    struct Network *network = network_alloc(nb_layers, layers_size);
    for (size_t i = 0; i < nb_layers; i++)
    {
        struct Layer *layer = &network->layers[i];
        matrix_load_inplace(f, layer->weight);
        matrix_load_inplace(f, layer->bias);
    }

    free(layers_size);
    fclose(f);
    (void) returnError;
    return network;
}
