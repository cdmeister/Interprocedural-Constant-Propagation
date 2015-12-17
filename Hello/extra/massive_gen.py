from __future__ import print_function
from sets import Set
import string
import random

def build_statement(var_def, var_def_list, operand_list):
    random_operand = operand_list[random.randint(0, len(operand_list) - 1)]
    return 'long ' + var_def + ' = ' + var_def_list[-1] + ' ' + random_operand + ' ' + var_def_list[random.randint(0, len(var_def_list) - 1)] + ';\n'

operands = ["+", "-", "*"]
defined_vars = ["a", "b", "c", "d"]

var_def_set = Set([])

# init with formal params
for i in range(len(defined_vars)):
    var_def_set.add(defined_vars[i])

function_body = ''
for i in range(10000):
    new_var = ''.join(random.choice(string.ascii_lowercase) for i in range(5))

    # add to set
    check_set = Set([new_var])

    if not check_set.issubset(var_def_set):
        var_def_set.add(new_var)
        function_body += build_statement(new_var, defined_vars, operands)
        defined_vars.append(new_var)

my_file = open('func_body.txt', 'w')

print(function_body, file=my_file)

my_file.close()
# print randomized_var_defs
