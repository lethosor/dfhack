#include "Console.h"
#include "Core.h"
#include "DataDefs.h"
#include "Export.h"
#include "PluginManager.h"
#include "PythonLib.h"

using namespace DFHack;

DFHACK_PLUGIN("python-test");
REQUIRE_GLOBAL(enabler);

command_result df_pytest (color_ostream &out, std::vector <std::string> & parameters);

DFhackCExport command_result plugin_init (color_ostream &out, std::vector <PluginCommand> &commands)
{
    commands.push_back(PluginCommand("pytest", "python test", df_pytest, false));
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown (color_ostream &out)
{
    return CR_OK;
}

command_result df_pytest (color_ostream &out, std::vector <std::string> & args)
{
    using namespace DFHack::Python;
    if (!isLoaded())
    {
        out.printerr("Not loaded\n");
        return CR_FAILURE;
    }
    int ret;
    if (args.size() >= 1)
        ret = PyRun_SimpleString(args[0].c_str());
    else
        ret = PyRun_SimpleString("print(2)");
    out << "exit code: " << ret << std::endl;
    fflush(stdout);
    return CR_OK;
}

