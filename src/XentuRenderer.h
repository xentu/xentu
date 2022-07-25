#ifndef XEN_RENDERER
#define XEN_RENDERER

#include <string>

#include "Xentu.h"
#include "XentuConfig.h"

namespace xen
{
	class XentuRenderer
	{
		public:
			XentuRenderer(const XentuConfig* config);
			~XentuRenderer();
			virtual int create_window();
			virtual int create_window_ex(std::string title, int x, int y, int width, int height, int mode);
			virtual bool is_running();
			virtual void present();

		private:
			const XentuConfig* m_config;
			
	};
}

#endif