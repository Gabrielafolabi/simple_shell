#include "simpleshell.h"

/**
 * my_env - prints the current environment
 * @info: Structure containing potential arguments. Used to maintain
 *          constant function prototype.
 * Return: Always 0
 */
int my_env(info_t *info)
{
	print_list_str(info->env);
	return (0);
}

/**
 * _getenv - gets the value of an environ variable
 * @info: Structure containing potential arguments. Used to maintain
 * @name: env var name
 *
 * Return: the value
 */
char *_getenv(info_t *info, const char *name)
{
	list_t *node = info->env;
	char *pr;

	while (node)
	{
		pr = strt_with(node->str, name);
		if (pr && *pr)
			return (pr);
		node = node->next;
	}
	return (NULL);
}

/**
 * my_setenv - Initialize a new environment variable,
 *             or modify an existing one
 * @info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 *  Return: Always 0
 */
int my_setenv(info_t *info)
{
	if (info->argc != 3)
	{
		input_str("Incorrect number of arguements\n");
		return (1);
	}
	if (my_setenv(info, info->argv[1], info->argv[2]))
		return (0);
	return (1);
}

/**
 * my_unsetenv - Remove an environment variable
 * @info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 *  Return: Always 0
 */
int my_unsetenv(info_t *info)
{
	int a;

	if (info->argc == 1)
	{
		input_str("Too few arguements.\n");
		return (1);
	}
	for (a = 1; a <= info->argc; a++)
		my_unsetenv(info, info->argv[a]);

	return (0);
}

/**
 * env_list - populates env linked list
 * @info: Structure containing potential arguments. Used to maintain
 *          constant function prototype.
 * Return: Always 0
 */
int env_list(info_t *info)
{
	list_t *node = NULL;
	size_t a;

	for (a = 0; environ[a]; a++)
		add_node_end(&node, environ[a], 0);
	info->env = node;
	return (0);
}

