#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

void transform_doc(int);

void put_string_char(char c)
{
    switch (c) {
    case '\b':
        fputs("\\b", stdout);
        break;
    case '\f':
        fputs("\\f", stdout);
        break;
    case '\n':
        fputs("\\n", stdout);
        break;
    case '\r':
        fputs("\\r", stdout);
        break;
    case '\t':
        fputs("\\t", stdout);
        break;
    case '"':
    case '\\':
        putchar('\\');
    default:
        putchar(c);
    }
}

void die(const char* msg)
{
    perror(msg);
    exit(1);
}

void transform_string(void)
{
    putchar('"');
    int string_count;
    fread(&string_count, sizeof(string_count), 1, stdin);
    for (int i = 0; i < string_count - 1; i++) {
        put_string_char(getchar());
    }
    getchar(); // discard \x00
    putchar('"');
}

void transform_int64(void)
{
    int64_t val;
    fread(&val, 8, 1, stdin);
    printf("%" PRId64, val);
}

void transform_int32(void)
{
    int32_t val;
    fread(&val, 4, 1, stdin);
    printf("%" PRId32, val);
}

void transform_double(void)
{
    double val;
    fread(&val, sizeof(val), 1, stdin);
    printf("%f", val);
}

void transform_objectid(void)
{
    int32_t val[3];
    fread(&val, 4, 3, stdin);
    printf("\"%x%x%x\"", val[0], val[1], val[2]);
}

void transform_binary(void)
{
    int32_t count;
    fread(&count, 4, 1, stdin);
    getchar();
    fputs("__binary", stdout);
    for (int i = 0; i < count; i++) {
        getchar();
    }
}

void transform_regex(void)
{
    fputs("\"/", stdout);
    for (char c = getchar(); c; c = getchar()) {
        put_string_char(c);
    }
    putchar('/');
    for (char c = getchar(); c; c = getchar()) {
        put_string_char(c);
    }
    putchar('"');
}

void transform_code_w_s(void)
{
    fputs("{\"__code\":", stdout);
    transform_string();
    fputs(",\"__scope\":", stdout);
    transform_doc(0);
    putchar('}');
}

void transform_value(char type)
{
    switch (type) {
    case '\x01':
        transform_double();
        break;
    case '\x02':
        transform_string();
        break;
    case '\x03':
        transform_doc(0);
        break;
    case '\x04':
        transform_doc(1);
        break;
    case '\x05':
        transform_binary();
        break;
    case '\x06':
        fputs("undefined", stdout);
        break;
    case '\x07':
        transform_objectid();
        break;
    case '\x08':
        fputs(getchar() ? "true" : "false", stdout);
        break;
    case '\x09':
        transform_int64();
        break;
    case '\x0A':
        fputs("null", stdout);
        break;
    case '\x0B':
        transform_regex();
        break;
    case '\x0C':
        transform_string();
        for (int i = 0; i < 12; i++) {
            getchar();
        }
        break;
    case '\x0D':
        transform_string();
        break;
    case '\x0E':
        transform_string();
        break;
    case '\x0F':
        transform_code_w_s();
        break;
    case '\x10':
        transform_int32();
        break;
    case '\x11':
        transform_int64();
        break;
    case '\x12':
        transform_int64();
        break;
    case '\xFF':
        fputs("\"__minkey\"", stdout);
        break;
    case '\x7F':
        fputs("\"__maxkey\"", stdout);
        break;
    default:
        die("Unrecognized element type");
    }
}

void transform_doc(int is_array)
{
    putchar(is_array ? '[' : '{');

    getchar(); getchar(); getchar(); getchar(); // discard count field

    for (int initial = 1; ; initial = 0) {
        char type = getchar();
        if (!type) {
            break;  // end of doc
        }

        if (!initial) {
            putchar(',');
        }
        if (!is_array) {
            putchar('"');
        }
        char namechar = getchar();
        while (namechar) {
            if (!is_array) {
                put_string_char(namechar);
            }
            namechar = getchar();
        }
        if (!is_array) {
            fputs("\":", stdout);
        }
        transform_value(type);
    }

    putchar(is_array ? ']' : '}');
}

int main(int argc, char** argv)
{
    while (!feof(stdin) && !ferror(stdin)) {
        transform_doc(0);
        putchar('\n');
    }
    exit(0);
}

