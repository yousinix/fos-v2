#ifndef FOS_KERN_MONITOR_H
#define FOS_KERN_MONITOR_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

// Function to activate the kernel command prompt
void run_command_prompt();
void command_prompt_readline(const char *, char *);

// Declaration of functions that implement command prompt commands.
int command_help(int , char **);
int command_kernel_info(int , char **);
int command_calc_space(int number_of_arguments, char **arguments);
int command_run_program(int argc, char **argv);
int command_readmem_k(int number_of_arguments, char **arguments);
int command_writemem_k(int number_of_arguments, char **arguments);
int command_kill_program(int number_of_arguments, char **arguments);

int commnad_load_env(int number_of_arguments, char **arguments);
int command_run_all(int number_of_arguments, char **arguments);
int command_print_all(int number_of_arguments, char **arguments);
int command_kill_all(int number_of_arguments, char **arguments);


#endif	// !FOS_KERN_MONITOR_H
