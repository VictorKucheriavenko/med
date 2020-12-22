#define \
debug()\
 printf("debug:%s:%d\n", __func__, __LINE__);\

#define \
priv(var, f)\
 fprintf(stderr, #var":%"#f"\n", (var));\

#define \
dpriv(var, f)\
 dprint(#var":%"#f"\n", (var));
