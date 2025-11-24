#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "expressao.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int isOperator(const char *tok)
{
    return (strcmp(tok, "+") == 0 ||
            strcmp(tok, "-") == 0 ||
            strcmp(tok, "*") == 0 ||
            strcmp(tok, "/") == 0 ||
            strcmp(tok, "%") == 0 ||
            strcmp(tok, "^") == 0);
}

static int isUnaryFunc(const char *tok)
{
    return (strcmp(tok, "raiz") == 0 ||
            strcmp(tok, "sen") == 0 ||
            strcmp(tok, "cos") == 0 ||
            strcmp(tok, "tg") == 0 ||
            strcmp(tok, "log") == 0);
}

static int isNumberToken(const char *tok)
{
    if (!tok || *tok == '\0')
        return 0;
    char *endptr;
    strtod(tok, &endptr);
    return (*endptr == '\0');
}

float getValorPosFixa(char *StrPosFixa)
{
    if (!StrPosFixa)
        return NAN;

    char buffer[1024];
    strncpy(buffer, StrPosFixa, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    double stackv[1024];
    int top = 0;

    char *tok = strtok(buffer, " ");
    while (tok)
    {
        if (isNumberToken(tok))
        {
            double v = strtod(tok, NULL);
            if (top >= 1024)
                return NAN;
            stackv[top++] = v;
        }
        else if (isOperator(tok))
        {
            if (top < 2)
                return NAN;
            double b = stackv[--top];
            double a = stackv[--top];
            double res = 0.0;
            if (strcmp(tok, "+") == 0)
                res = a + b;
            else if (strcmp(tok, "-") == 0)
                res = a - b;
            else if (strcmp(tok, "*") == 0)
                res = a * b;
            else if (strcmp(tok, "/") == 0)
            {
                if (b == 0.0)
                    return NAN;
                res = a / b;
            }
            else if (strcmp(tok, "%") == 0)
            {
                if (b == 0.0)
                    return NAN;
                res = fmod(a, b);
            }
            else if (strcmp(tok, "^") == 0)
            {
                res = pow(a, b);
            }
            stackv[top++] = res;
        }
        else if (isUnaryFunc(tok))
        {
            if (top < 1)
                return NAN;
            double a = stackv[--top];
            double res = 0.0;
            if (strcmp(tok, "raiz") == 0)
            {
                if (a < 0.0)
                    return NAN;
                res = sqrt(a);
            }
            else if (strcmp(tok, "sen") == 0)
            {
                res = sin(a * M_PI / 180.0);
            }
            else if (strcmp(tok, "cos") == 0)
            {
                res = cos(a * M_PI / 180.0);
            }
            else if (strcmp(tok, "tg") == 0)
            {
                double rad = a * M_PI / 180.0;
                res = tan(rad);
            }
            else if (strcmp(tok, "log") == 0)
            {
                if (a <= 0.0)
                    return NAN;
                res = log10(a);
            }
            stackv[top++] = res;
        }
        else
        {

            return NAN;
        }
        tok = strtok(NULL, " ");
    }

    if (top != 1)
        return NAN;
    return (float)stackv[0];
}

char *getFormaInFixa(char *Str)
{
    if (!Str)
        return NULL;

    char *stacks[1024];
    int top = 0;

    char buffer[1024];
    strncpy(buffer, Str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *tok = strtok(buffer, " ");
    while (tok)
    {
        if (isNumberToken(tok))
        {

            char *s = (char *)malloc(strlen(tok) + 1);
            if (!s)
                goto err;
            strcpy(s, tok);
            stacks[top++] = s;
        }
        else if (isUnaryFunc(tok))
        {

            if (top < 1)
                goto err;
            char *a = stacks[--top];

            size_t n = strlen(tok) + 1 + strlen(a) + 2;
            char *res = (char *)malloc(n);
            if (!res)
            {
                free(a);
                goto err;
            }
            sprintf(res, "%s(%s)", tok, a);
            free(a);
            stacks[top++] = res;
        }
        else if (isOperator(tok))
        {

            if (top < 2)
                goto err;
            char *right = stacks[--top];
            char *left = stacks[--top];

            size_t n = strlen(left) + strlen(right) + strlen(tok) + 4 + 1;
            char *res = (char *)malloc(n);
            if (!res)
            {
                free(left);
                free(right);
                goto err;
            }
            sprintf(res, "(%s%s%s)", left, tok, right);
            free(left);
            free(right);
            stacks[top++] = res;
        }
        else
        {
            goto err;
        }

        tok = strtok(NULL, " ");
    }

    if (top != 1)
        goto err;

    char *final = stacks[0];
    if (final[0] == '(')
    {
        size_t len = strlen(final);
        if (len >= 2 && final[len - 1] == ')')
        {
            int depth = 0;
            int removable = 1;
            for (size_t i = 0; i < len; ++i)
            {
                if (final[i] == '(')
                    depth++;
                else if (final[i] == ')')
                    depth--;
                if (i > 0 && i < len - 1 && depth == 0)
                {
                    removable = 0;
                    break;
                }
            }
            if (removable && depth == 0)
            {
                char *trim = (char *)malloc(len - 1);
                if (!trim)
                {
                    free(final);
                    return NULL;
                }
                strncpy(trim, final + 1, len - 2);
                trim[len - 2] = '\0';
                free(final);
                final = trim;
            }
        }
    }

    return final;

err:
    for (int i = 0; i < top; ++i)
    {
        free(stacks[i]);
    }
    return NULL;
}