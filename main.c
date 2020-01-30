 #include "add.h"

void compress(FILE *input, FILE *output)
{
    int *counts = calloc(256, sizeof(int));
    cnt(input, counts);

    queue *q = NULL;

    for (int i = 0; i < 256; ++i)
    {
        unsigned char byte = (unsigned char) i;
        if (counts[byte] != 0)
        {
            tree *root = malloc(sizeof(tree));
            root->priority = counts[byte];
            root->value = byte;
            root->right = NULL;
            root->left = NULL;
            q = push(q, root);
        }
    }

    free(counts);

    if (q == NULL)
    {
        return;
    }

    if (q->next == NULL)
    {
        fputc(0, output);
        fputc(q->value->value, output);
        fwrite(&q->value->priority, sizeof(int), 1, output);
        return;
    }

    while (q->next != NULL)
    {
        tree *a;
        tree *b;

        q = pop(q, &a);
        q = pop(q, &b);

        tree *root = malloc(sizeof(tree));
        root->priority = a->priority + b->priority;
        root->right = b;
        root->left = a;

        q = push(q, root);
    }

    bitarray **table = calloc(sizeof(bitarray *), 256);
    bitarray *current = create_arr();
    build_table(table, q->value, current);
    free_bitarr(current);


    bitarray *out = create_arr();
    print_tree(q->value, out);

    free_tree(q->value);
    free(q);

    fseek(input, 0, SEEK_SET);

    const int size = 1024 * 3;
    unsigned char *read = malloc(sizeof(char) * size);
    int read_count = fread(read, 1, size, input);

    while (read_count != 0)
    {
        for (int i = 0; i < read_count; i++)
        {
            push_bitarr(out, table[read[i]]);
        }

        read_count = fread(read, 1, size, input);
    }

    free(read);
    for(int i = 0; i<256; i++)
    {
        free_bitarr(table[i]);
    }
    free(table);

    int nulll_count = 0;

    while (out->last_bit % 8 != 0)
    {
        push_bit(out, 0);
        nulll_count++;
    }
    push_byte(out, nulll_count);

    fwrite(out->data, 1, out->last_bit / 8, output);

    free_bitarr(out);

}

void decompress(FILE *compressed, FILE *decompressed)
{
    if (fgetc(compressed) == 0)
    {
        unsigned char symbol = fgetc(compressed);
        int cnt;
        fread(&cnt, sizeof(int), 1, compressed);

        for (int i = 0; i < cnt; i++)
            fputc(symbol, decompressed);

        return;
    }

    fseek(compressed, 0, SEEK_SET);

    bitarray *arr = load_arr(compressed);
    if (arr->last_bit == 0)
        return;

    unsigned char delete = arr->data[arr->last_bit/8 - 1];
    arr->last_bit = arr->last_bit - 8 - delete;

    tree *root = load_tree(arr);

    while (arr->last_bit > arr->skipped_bits)
    {
        read_symbol(arr, root, decompressed);
    }

    free(arr->data);
    free(arr);
    free_tree(root);
}

int main(int argc, char *argv[])
{
    if (argc == 4)
    {
        FILE *input, *output;
        if ((input = fopen(argv[2], "r")) == NULL)
        {
            printf("Cannot open %s \n", argv[2]);
            return 1;
        }
        if ((output = fopen(argv[3], "wb")) == NULL)
        {
            printf("Cannot open %s \n", argv[3]);
            fclose(input);
            return 1;
        }

        if ((strcmp(argv[1], "-c") == 0))
        {
            compress(input, output);
            fclose(input);
            fclose(output);
        }
        else if ((strcmp(argv[1], "-d") == 0))
        {
            decompress(input, output);
            fclose(input);
            fclose(output);
        }
        else
        {
            printf("Huffman.exe -c/-d input.txt output.txt \t to compress/decompress file\n");
            fclose(input);
            fclose(output);
        }
    }
    else
    {
        printf("Huffman.exe -c/-d input.txt output.txt \t to compress/decompress file\n");
    }
    return 0;
}