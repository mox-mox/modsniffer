// Adapted from <https://stackoverflow.com/q/44095001/4360539>

#include <iostream>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>


#include <vector>
//#include <filesystem>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <fstream>

std::vector < std::string > get_polybar_message_queues(void)
{
	std::vector < std::string > retval;
	for( auto& p: fs::directory_iterator("/tmp"))
	{
		std::string path =  p.path().string();
		if( path.find("/tmp/polybar_mqueue.") != std::string::npos )
		{
			retval.push_back(path);
		}
	}

	return retval;
}


int main(int argc, char* argv[])
{
	// First argument is the keysym of the key which should trigger polybar
	if( argc < 2 )
	{
		std::cerr<<"Please provide the keysym to watch."<<std::endl;
		exit(EXIT_FAILURE);
	}
	std::string watched_mod = argv[1];



	// Get all polybar message queues
	std::vector < std::string > queues = get_polybar_message_queues();
	for( auto& str : queues )
		std::cout<<str<<std::endl;



	// Initialise the X11 connection
	Display* display = XOpenDisplay(NULL);
	Window win;

	win = DefaultRootWindow(display);
	XIEventMask* m = new XIEventMask;

	m->deviceid = XIAllMasterDevices;
	m->mask_len = XIMaskLen(XI_LASTEVENT);
	m->mask = (unsigned char*) calloc(m->mask_len, sizeof(char));

	XISetMask(m->mask, XI_RawKeyPress);
	XISetMask(m->mask, XI_RawKeyRelease);

	XISelectEvents(display, win, m, 1);

	XSync(display, False);

	while( true )
	{
		XEvent ev;
		XGenericEventCookie* cookie = (XGenericEventCookie*) &ev.xcookie;
		XNextEvent(display, (XEvent*) &ev);

		if( XGetEventData(display, cookie) && (cookie->type == GenericEvent))
		{
			switch( cookie->evtype )
			{
				case XI_RawKeyRelease:
					__attribute__((fallthrough));
				case XI_RawKeyPress:
					XIRawEvent* ev = static_cast < XIRawEvent*> (cookie->data);

					// Get the keysym
					KeySym keysym = XkbKeycodeToKeysym(display, ev->detail, 0, 0);
					if( NoSymbol == keysym ) continue;
					char* keysym_str = XKeysymToString(keysym);
					if( NULL == keysym_str ) continue;

					//std::cout<<(cookie->evtype == XI_RawKeyPress ? "+" : "-")<<keysym_str<<std::endl;

					// Act only on the watched key
					if( !watched_mod.compare(keysym_str))
					{
						for( auto& queue : queues )
						{
							std::ofstream queue_file(queue);
							if( !queue_file.is_open())
							{
								std::cerr<<"Could not write to queue."<<std::endl;
								exit(EXIT_FAILURE);
							}
							// Show on key press, hide on key release
							queue_file<<"cmd:"<<(cookie->evtype == XI_RawKeyPress ? "show" : "hide")<<std::endl;
						}
					}

					break;
			}
		}


		XFreeEventData(display, cookie);
	}

	XDestroyWindow(display, win);

	return 0;
}
