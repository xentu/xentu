#ifndef XEN_MACHINE
#define XEN_MACHINE

#include "Xentu.h"

namespace xen
{
	class XentuMachine
	{
		public:
			XentuMachine(const size_t argc, const char *argv[]);
			~XentuMachine();
			virtual int run(const std::string entry_point);
			virtual int trigger(const std::string event_name);
			std::string read_text_file(std::string file);

		protected:
			const size_t arg_count;
			const char* arg_values[MAX_ARGV];
	};
}

#endif