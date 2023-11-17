#include "simpleshell.h"

/**
 * prompt - main shell prompt
 * @info: the parameter & return info struct
 * @agv: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int prompt(info_t *info, char **agv)
{
	ssize_t ret = 0;
	int builtin_retain = 0;

	while (ret != -1 && builtin_retain != -2)
	{
		clear_info(info);
		if (interactive(info))
			_puts("$Troy-Gabe$ ");
		_eputchar(BUF_FLUSH);
		ret = get_input(info);
		if (ret != -1)
		{
			set_info(info, agv);
			builtin_retain = findbuiltin(info);
			if (builtin_retain == -1)
				fnd_comd(info);
		}
		else if (interactive(info))
			_putchar('\n');
		free_info(info, 0);
	}
	write_history(info);
	free_info(info, 1);
	if (!interactive(info) && info->status)
		exit(info->status);
	if (builtin_retain == -2)
	{
		if (info->err_num == -1)
			exit(info->status);
		exit(info->err_num);
	}
	return (builtin_retain);
}

/**
 * findbuiltin - finds a builtin command
 * @info: the parameter & return info struct
 *
 * Return: -1 builtin not found,
 *			0 builtin successfully executed,
 *			1 builtin found not successful,
 *			-2 signals exit()
 */
int findbuiltin(info_t *info)
{
	int i, built_in_retain = -1;
	builtin_table builtintbl[] = {
		{"exit", _myexit},
		{"env", _myenv},
		{"help", _myhelp},
		{"history", _myhistory},
		{"setenv", _mysetenv},
		{"unsetenv", _myunsetenv},
		{"cd", _mycd},
		{"alias", _myalias},
		{NULL, NULL}
	};

	for (i = 0; builtintbl[i].type; i++)
		if (_strcmp(info->argv[0], builtintbl[i].type) == 0)
		{
			info->line_count++;
			built_in_retain = builtintbl[i].func(info);
			break;
		}
	return (built_in_retain);
}

/**
 * fnd_comd - finds a command in PATH
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void fnd_comd(info_t *info)
{
	char *path = NULL;
	int a, b;

	info->path = info->argv[0];
	if (info->linecount_flag == 1)
	{
		info->line_count++;
		info->linecount_flag = 0;
	}
	for (a = 0, b = 0; info->arg[a]; a++)
		if (!is_delim(info->arg[a], " \t\n"))
			b++;
	if (!b)
		return;

	path = find_path(info, _getenv(info, "PATH="), info->argv[0]);
	if (path)
	{
		info->path = path;
		fork_comd(info);
	}
	else
	{
		if ((interactive(info) || _getenv(info, "PATH=")
			|| info->argv[0][0] == '/') && is_cmd(info, info->argv[0]))
			fork_comd(info);
		else if (*(info->arg) != '\n')
		{
			info->status = 127;
			print_error(info, "This file doesn't exist\n");
		}
	}
}

/**
 * fork_comd - forks a an exec thread to run cmd
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void fork_comd(info_t *info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{

		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(info->path, info->argv, get_environ(info)) == -1)
		{
			free_info(info, 1);
			if (errno == EACCES)
				exit(126);
			exit(1);
		}

	}
	else
	{
		wait(&(info->status));
		if (WIFEXITED(info->status))
		{
			info->status = WEXITSTATUS(info->status);
			if (info->status == 126)
				print_error(info, "Permission denied\n");
		}
	}
}

