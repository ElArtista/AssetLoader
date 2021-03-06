#include "assets/shader/shaderload.h"
#include "assets/fileload.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Custom version of standard isspace, to avoid MSVC checks */
static int is_space(const char c)
{
    return c == ' '
        || c == '\t'
        || c == '\n'
        || c == '\v'
        || c == '\f'
        || c == '\r';
}

/* Load proxy */
static int shader_file_load(const char* uri, struct shader_load_settings* settings, unsigned char** data_buf)
{
    int ret = 0;
    *data_buf = 0;
    if (settings->load_type == SHADER_LOAD_FILE) {
        /* Check file for existence */
        long filesz = filesize(uri);
        if (filesz != -1) {
            /* Gather file contents */
            *data_buf = malloc(filesz + 1);
            read_file_to_mem(uri, *data_buf, filesz);
            *(*data_buf + filesz) = '\0';
            ret = 1;
        }
    } else if (settings->load_type == SHADER_LOAD_CUSTOM) {
        ret = settings->load_fn(settings->userdata, uri, data_buf);
    }
    return ret;
}

static const char* INCLUDE_DIRECTIVE = "#include ";
static int INCLUDE_DIRECTIVE_SZ = 9;

/* Waits whitespace stripped input */
static int shader_include_param_valid(const char* param)
{
    /* Size must at least be 2 for surrounding quoting plus 1 for non empty uri */
    size_t param_sz = strlen(param);
    if (param_sz < 3)
        return 0;

    /* Check for "" and <> quotes */
    if (param[0] == '"' && param[param_sz - 1] == '"')
        return 1;
    else if (param[0] == '<' && param[param_sz - 1] == '>')
        return 1;

    /* No valid quoting scheme found */
    return 0;
}

/* To unquote the include param */
static void remove_surrounding_quotes(char* str)
{
    size_t nlen = strlen(str) - 2;
    memmove(str, str + 1, nlen);
    *(str + nlen) = '\0';
}

static const char* shader_load_error_str(const char* file, int line, const char* error, const char* error_desc)
{
    size_t buf_sz = 512;
    char* buf = calloc(buf_sz, 1);
    snprintf(buf, buf_sz,"Error(%s:%d): %s %s", file, line, error, error_desc);
    return buf;
}

static const char* shader_load_error_code_str(int ec)
{
    const char* str = "";
    switch (ec) {
        case 1:
            str = "invalid #include param";
            break;
        case 2:
            str = "could not find file";
            break;
        case 3:
            str = "error while including file";
            break;
    }
    return str;
}

static const char* shader_read(const char* uri, int* ec, struct shader_load_settings* settings)
{
    /* Load source from uri */
    char* src = 0;
    int fexists = shader_file_load(uri, settings, (unsigned char**)&src);
    if (!fexists)
        return 0;

    /* Extract base path from uri */
    char* uri_basepath = calloc(strlen(uri) + 1, 1);
    const char* bp_end = strrchr(uri, '/');
    strncpy(uri_basepath, uri, bp_end - uri);

    /* Allocate initial preprocessed source buffer to be returned */
    size_t src_sz = strlen(src);
    size_t preproc_src_sz = src_sz + 2;
    char* preproc_src = calloc(preproc_src_sz, 1);

    /* Used also for error reporting */
    *ec = 0;
    char* include_uri;

    /* Iterate the source line by line */
    int line = 1;
    char* cur = src;
    while (cur < src + src_sz) {
        /* Set start and end of line points */
        char* sol = cur, *eol = cur;
        while (eol && *eol != '\n') ++eol;
        if (eol)
            *eol = '\0';

        /* Skip leading whitespace */
        while (cur && is_space(*cur)) ++cur;

        /* Check for preprocessor directives */
        if (strncmp(cur, INCLUDE_DIRECTIVE, INCLUDE_DIRECTIVE_SZ) == 0) {
            /* Include param */
            include_uri = cur + INCLUDE_DIRECTIVE_SZ;
            while (cur && !is_space(*cur)) ++cur;
            *cur = '\0';

            /* Check validity of param */
            if (!shader_include_param_valid(include_uri)) {
                *ec = 1;
                break;
            }

            /* Remove quotes from include */
            remove_surrounding_quotes(include_uri);

            /* Construct load path using basepath */
            char* full_uri = calloc(strlen(uri_basepath) + 1 + strlen(include_uri) + 1, 1);
            strcat(full_uri, uri_basepath);
            strcat(full_uri, "/");
            strcat(full_uri, include_uri);

            /* Read subfile (recurse) */
            int sub_ec = 0;
            const char* subcontent = shader_read(full_uri, &sub_ec, settings);
            free(full_uri);
            if (!subcontent) {
                if (sub_ec) {
                    /* Error in subfile */
                    *ec = 3;
                } else {
                    /* Given file is missing */
                    *ec = 2;
                }
                break;
            }

            /* Calc new preproc string size */
            size_t subcontent_sz = strlen(subcontent);
            size_t npreproc_src_sz = preproc_src_sz + subcontent_sz - (eol - sol) + 1;
            if (npreproc_src_sz > preproc_src_sz) {
                /* Additional space needed, reallocate */
                preproc_src = realloc(preproc_src, npreproc_src_sz);
                preproc_src_sz = npreproc_src_sz;
            }
            /* Append contents */
            strcat(preproc_src, subcontent);
            strcat(preproc_src, "\n");
            free((void*)subcontent);
        } else {
            /* Just copy the full line */
            strcat(preproc_src, sol);
            strcat(preproc_src, "\n");
        }

        /* Advance current position */
        cur = eol + 1;
        /* Increase line count */
        ++line;
    }

    if (*ec) {
        const char* err_str = shader_load_error_code_str(*ec);
        const char* err = shader_load_error_str(bp_end + 1, line, err_str, include_uri);
        if (settings->error_cb)
            settings->error_cb(settings->userdata, err);
        free((void*)err);

        free(preproc_src);
        preproc_src = 0;
    }

    free(uri_basepath);
    free((void*)src);
    return preproc_src;
}

const char* shader_load(const char* uri, struct shader_load_settings* settings)
{
    int ec = 0;
    return shader_read(uri, &ec, settings);
}
