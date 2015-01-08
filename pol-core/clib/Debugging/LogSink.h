#ifndef LOG_SINK_H
#define LOG_SINK_H

#include <boost/noncopyable.hpp>
#include "../../lib/format/format.h" //TODO: simplify this after cleanup of build system
#include <string>

namespace Pol{ namespace Clib{ namespace Logging{

class LogSink : boost::noncopyable
{
public:
	LogSink();
	virtual ~LogSink();

	virtual void sink(fmt::Writer* msg) = 0;
	virtual void sink(fmt::Writer* msg, std::string id) = 0;

    /**
     * Helper function to print timestamp into stream
     */
	static void printCurrentTimeStamp( std::ostream &stream );

    /**
     * Formats and returns current time as std::string
     */
	static std::string getLoggingTimeStamp();
};

}}} // namespaces

#endif
