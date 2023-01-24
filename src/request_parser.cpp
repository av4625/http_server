#include "request_parser.hpp"
#include "request_data.hpp"

namespace http
{

request_parser::request_parser() : state_(state::method_start)
{
}

void request_parser::reset()
{
    state_ = state::method_start;
}

boost::tribool request_parser::consume(request_data& req, const char input)
{
    switch (state_)
    {
        case state::method_start:
        {
            if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return false;
            }
            else
            {
                state_ = state::method;
                req.method.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::method:
        {
            if (input == ' ')
            {
                state_ = state::uri;
                return boost::indeterminate;
            }
            else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return false;
            }
            else
            {
                req.method.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::uri:
        {
            if (input == ' ')
            {
                state_ = state::http_version_h;
                return boost::indeterminate;
            }
            else if (input == '?')
            {
                state_ = state::query_parameter_name;
                req.query_parameters.push_back(query_parameter());
                return boost::indeterminate;
            }
            else if (is_ctl(input))
            {
                return false;
            }
            else
            {
                req.uri.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::query_parameter_name:
        {
            if (input == '=')
            {
                state_ = state::query_parameter_value;
                return boost::indeterminate;
            }
            else if (req.query_parameters.empty() || is_ctl(input) || is_tspecial(input))
            {
                return false;
            }
            else
            {
                req.query_parameters.back().name.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::query_parameter_value:
        {
            if (input == '&')
            {
                state_ = state::query_parameter_name;
                req.query_parameters.push_back(query_parameter());
                return boost::indeterminate;
            }
            else if (input == ' ')
            {
                state_ = state::http_version_h;
                return boost::indeterminate;
            }
            else if (req.query_parameters.empty() || is_ctl(input) || is_tspecial(input))
            {
                return false;
            }
            else
            {
                req.query_parameters.back().value.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::http_version_h:
        {
            if (input == 'H')
            {
                state_ = state::http_version_t_1;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_t_1:
        {
            if (input == 'T')
            {
                state_ = state::http_version_t_2;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_t_2:
        {
            if (input == 'T')
            {
                state_ = state::http_version_p;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_p:
        {
            if (input == 'P')
            {
                state_ = state::http_version_slash;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_slash:
        {
            if (input == '/')
            {
                req.http_version_major = 0;
                req.http_version_minor = 0;
                state_ = state::http_version_major_start;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_major_start:
        {
            if (is_digit(input))
            {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                state_ = state::http_version_major;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_major:
        {
            if (input == '.')
            {
                state_ = state::http_version_minor_start;
                return boost::indeterminate;
            }
            else if (is_digit(input))
            {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_minor_start:
        {
            if (is_digit(input))
            {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                state_ = state::http_version_minor;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::http_version_minor:
        {
            if (input == '\r')
            {
                state_ = state::expecting_newline_1;
                return boost::indeterminate;
            }
            else if (is_digit(input))
            {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::expecting_newline_1:
        {
            if (input == '\n')
            {
                state_ = state::header_line_start;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::header_line_start:
        {
            if (input == '\r')
            {
                state_ = state::expecting_newline_3;
                return boost::indeterminate;
            }
            else if (!req.headers.empty() && (input == ' ' || input == '\t'))
            {
                state_ = state::header_lws;
                return boost::indeterminate;
            }
            else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return false;
            }
            else
            {
                req.headers.push_back(header());
                req.headers.back().name.push_back(input);
                state_ = state::header_name;
                return boost::indeterminate;
            }
        }
        case state::header_lws:
        {
            if (input == '\r')
            {
                state_ = state::expecting_newline_2;
                return boost::indeterminate;
            }
            else if (input == ' ' || input == '\t')
            {
                return boost::indeterminate;
            }
            else if (is_ctl(input))
            {
                return false;
            }
            else
            {
                state_ = state::header_value;
                req.headers.back().value.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::header_name:
        {
            if (input == ':')
            {
                state_ = state::space_before_header_value;
                return boost::indeterminate;
            }
            else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
            {
                return false;
            }
            else
            {
                req.headers.back().name.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::space_before_header_value:
        {
            if (input == ' ')
            {
                state_ = state::header_value;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::header_value:
        {
            if (input == '\r')
            {
                state_ = state::expecting_newline_2;
                return boost::indeterminate;
            }
            else if (is_ctl(input))
            {
                return false;
            }
            else
            {
                req.headers.back().value.push_back(input);
                return boost::indeterminate;
            }
        }
        case state::expecting_newline_2:
        {
            if (input == '\n')
            {
                state_ = state::header_line_start;
                return boost::indeterminate;
            }
            else
            {
                return false;
            }
        }
        case state::expecting_newline_3:
        {
            return (input == '\n');
        }
        default:
        {
            return false;
        }
    }
}

bool request_parser::is_char(const int c)
{
    return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(const int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(const int c)
{
    switch (c)
    {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t':
            return true;
        default:
            return false;
    }
}

bool request_parser::is_digit(const int c)
{
    return c >= '0' && c <= '9';
}

}
