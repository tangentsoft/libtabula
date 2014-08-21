/***********************************************************************
 options.cpp - Implements the Option class hierarchy.

 Copyright © 2007-2009, 2014 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS file in the top directory of the distribution for details.

 This file is part of libtabula.

 libtabula is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 libtabula is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with libtabula; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#define LIBTABULA_NOT_HEADER
#include "options.h"

#include "dbdriver.h"


namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// We're hiding all the Option subclass internals from Doxygen.  All the
// upper-level classes are documented fully, and each leaf class itself
// is documented.  It's just the set() methods we're refusing to document
// over and over again.

Option::Error
CompressOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ConnectTimeoutOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
FoundRowsOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
GuessConnectionOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
IgnoreSpaceOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
InitCommandOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
InteractiveOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
LocalFilesOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
LocalInfileOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
MultiResultsOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
MultiStatementsOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
NamedPipeOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
NoSchemaOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ProtocolOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ReadDefaultFileOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ReadDefaultGroupOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ReadTimeoutOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ReconnectOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
ReportDataTruncationOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
SecureAuthOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
SetCharsetDirOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
SetCharsetNameOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
SetClientIpOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
SharedMemoryBaseNameOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
SslOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
UseEmbeddedConnectionOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
UseRemoteConnectionOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}


Option::Error
WriteTimeoutOption::set(DBDriver* dbd)
{
	return dbd->set_option_impl(*this);
}

#endif // !defined(DOXYGEN_IGNORE)

} // end namespace libtabula
