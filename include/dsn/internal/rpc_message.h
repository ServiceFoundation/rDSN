/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Microsoft Corporation
 * 
 * -=- Robust Distributed System Nucleus (rDSN) -=- 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
# pragma once

# include <atomic>
# include <dsn/internal/dsn_types.h>
# include <dsn/internal/extensible_object.h>
# include <dsn/internal/task_spec.h>
# include <dsn/cpp/auto_code.h>

namespace dsn 
{
    typedef struct dsn_buffer_t // binary compatible with WSABUF on windows
    {
        unsigned long length;
        char          *buffer;
    } dsn_buffer_t;

    typedef struct message_header
    {
        int32_t       hdr_crc32;
        int32_t       body_crc32;
        int32_t       body_length;
        int32_t       version;
        uint64_t      id;
        uint64_t      rpc_id;
        char          rpc_name[DSN_MAX_TASK_CODE_NAME_LENGTH];

        // info from client => server
        union
        {
            struct
            {
                int32_t  timeout_ms;
                int32_t  hash;
                uint16_t port;
            } client;

            struct
            {
                int32_t  error;
            } server;
        };
    } message_header;
            
    class message_ex : 
        public ref_object, 
        public extensible_object<message_ex, 4>
    {
    public:
        message_header         *header;
        std::vector<blob>      buffers; // header included for *send* message, 
                                        // header not included for *recieved* 

        // by rpc and network
        rpc_server_session_ptr server_session;
        dsn_address_t          from_address;
        dsn_address_t          to_address;
        uint16_t               local_rpc_code;

    public:        
        //message_ex(blob bb, bool parse_hdr = true); // read 
        ~message_ex();

        //
        // utility routines
        //
        bool is_right_header() const;
        bool is_right_body() const;
        error_code error() const { return header->server.error; }        
        static uint64_t new_id() { return ++_id; }
        static bool is_right_header(char* hdr);
        static int  get_body_length(char* hdr)
        {
            return ((message_header*)hdr)->body_length;
        }

        //
        // routines for create messages
        //
        static message_ex* create_receive_message(blob& data);
        static message_ex* create_receive_message(blob& data, message_header* header, std::shared_ptr<char>& header_holder);
        static message_ex* create_request(dsn_task_code_t rpc_code, int timeout_milliseconds = 0, int hash = 0);
        message_ex* create_response();


        //
        // routines for buffer management
        //        
        void write_next(void** ptr, size_t* size, size_t min_size);
        void write_commit(size_t size);
        bool read_next(void** ptr, size_t* size);
        void read_commit(size_t size);
        size_t body_size() { return (size_t)header->body_length; }
        void* rw_ptr(size_t offset_begin);
        void seal(bool crc_required);

    private:
        message_ex();
        void prepare_buffer_header();

    private:        
        static std::atomic<uint64_t> _id;

    private:
        // by msg read & write
        int                    _rw_index;
        int                    _rw_offset;
        bool                   _rw_committed;
        bool                   _is_read;
        std::shared_ptr<char>  _header_holder;
    };
    
//
//struct dsn_message_header_helper
//{
//    //static void marshall(message_header* hdr, binary_writer& writer);
//    //static void unmarshall(message_header* hdr, binary_reader& reader);   
//    static bool is_right_header(char* hdr);
//    static int  get_body_length(char* hdr)
//    {
//        return ((message_header*)hdr)->body_length;
//    }
//};

//
//class rpc_server_session;
//class message : public ref_object, public extensible_object<message, 4>, public ::dsn::tools::memory::tallocator_object
//{
//public:
//    message(); // write             
//    message(blob bb, bool parse_hdr = true); // read 
//    virtual ~message();
//
//    //
//    // routines for request and response
//    //
//    static dsn_message_t create_request(dsn_task_code_t rpc_code, int timeout_milliseconds = 0, int hash = 0);
//    dsn_message_t create_response();
//
//    //
//    // routines for reader & writer
//    //
//    binary_reader& reader() { return *_reader; }
//    binary_writer& writer() { return *_writer; }
//
//    //
//    // meta info
//    //
//    void seal(bool fillCrc, bool is_placeholder = false);
//    message_header& header() { return _msg.hdr; }
//    int  total_size() const { return _is_read() ? _reader->total_size() : _writer->total_size(); }
//    bool _is_read() const { return _reader != nullptr; }
//    error_code error() const { error_code ec; ec.set(_msg.hdr.server.error); return ec; }
//    bool is_right_header() const;
//    bool is_right_body() const;
//    static uint64_t new_id() { return ++_id; }
//    rpc_server_session_ptr& server_session() { return _server_session; }
//    dsn_message_t c_msg() { return &_msg; }
//    static message* from_c_msg(dsn_message_t msg) {
//        return CONTAINING_RECORD(msg, message, _msg);
//    }
//
//private:            
//    void read_header();
//
//private:
//    dsn_message_t          _msg;
//    binary_reader          *_reader;
//    binary_writer          *_writer;
//    rpc_server_session_ptr _server_session;
//
//protected:
//    static std::atomic<uint64_t> _id;
//};
//
//DEFINE_REF_OBJECT(message)

} // end namespace
