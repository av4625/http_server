#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>

#include "header_field.hpp"
#include "method.hpp"

namespace http
{

// A class/wrapper to provide unmodifiable access to the request
class request
{
public:
    virtual ~request() = default;

    /**
     * @brief Get the method of the request.
     *
     * @returns The requests method.
    */
    virtual method get_method() const = 0;

    /**
     * @brief Get the endpoint of the request.
     *
     * @returns The requests endpoint.
    */
    virtual std::string get_endpoint() const = 0;

    /**
     * @brief Return whether the request has a specified query param.
     *
     * @param[in] key The param key to search for.
     *
     * @returns True if the request has the query param else false.
    */
    virtual bool has_query_param(const std::string& key) const = 0;

    /**
     * @brief Return the value of a specified query param.
     *
     * @param[in] key The param key to return the value for.
     *
     * @returns The specified query params value.
     *
     * @throws std::invalid_argument if the specified query param does not exist.
    */
    virtual std::string get_query_param(const std::string& key) const = 0;

    /**
     * @brief Return whether the request has a specified header using a string
     *        name.
     *
     * @param[in] name The header name to search for.
     *
     * @returns True if the request has the header else false.
    */
    virtual bool has_header(const std::string& name) const = 0;

    /**
     * @brief Return whether the request has a specified header using a field
     *        name.
     *
     * @param[in] name The header name to search for.
     *
     * @returns True if the request has the header else false.
    */
    virtual bool has_header(field name) const = 0;

    /**
     * @brief Return the value of a specified header using a string name.
     *
     * @param[in] name The header name to return the value for.
     *
     * @returns The specified headers value.
     *
     * @throws std::invalid_argument if the specified header does not exist.
    */
    virtual std::string get_header_value(const std::string& name) const = 0;

    /**
     * @brief Return the value of a specified header using a field name.
     *
     * @param[in] name The header name to return the value for.
     *
     * @returns The specified headers value.
     *
     * @throws std::invalid_argument if the specified header does not exist.
    */
    virtual std::string get_header_value(field name) const = 0;

    /**
     * @brief Return the body of the request as a string.
     *
     * @returns The body of the request as a string.
    */
    virtual const std::string& body() const = 0;
};

}

#endif
