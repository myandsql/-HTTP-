#include "http_request.h"

HttpRequest::HttpRequest(int socket): m_fd(socket) {
    m_status = STATUS::READ_HTTP_HEAD;
    m_end_status = END::NO;
    m_read_index = m_end_index = 0;
    m_string_index = 0;
}

HttpRequest::~HttpRequest() {
}

bool HttpRequest::init() {
    m_status = STATUS::READ_HTTP_HEAD;
    m_end_status = END::NO;
    // std::cout<<m_method.size()<<std::endl;
    m_method.clear();
    m_path.clear();
    m_protocolVersion.clear();

    m_values.clear();
//std::cout<<m_headers.size()<<std::endl;

    for(auto &x : m_headers) {
        m_headers.erase(x.first);
    }
    m_body.clear();
    // LOG_DEBUG("init success!");
    m_read_index = m_end_index = 0;
    m_string_index = 0;
    m_string_buff1.clear();
    m_string_buff2.clear();
    LOG_DEBUG("init success!");
    return true;
}

int HttpRequest::read_socket() {
    while(true) {
        int ret = read(m_fd, m_buffer, 1024);
        if(ret > 0) {
            m_end_index = ret;
            m_read_index = 0;
            return 1;
        } else if(ret == -1) {
            if(errno == EINTR)continue;
            else if(errno == EWOULDBLOCK || errno == EAGAIN)return -1;
            throw std::exception();
        } else if(ret == 0) {
            throw std::exception();
        }

    }
    return -1;
}

void HttpRequest::read_word(string &word) {
    while(true) {
        for(int i = m_read_index; i < m_end_index; i++, m_read_index++) {
            if(m_buffer[i] == ' ' || m_buffer[i] == ':' || m_buffer[i] == '?' || m_buffer[i] == '=' || m_buffer[i] == '\n' || m_buffer[i] == '&') {
                if(m_buffer[i] == '\n') {
                    m_read_index += 1;
                    word.pop_back();
                    m_end_status = END::LINE_END;
                } else {
                    m_read_index += 1;
                    m_end_status = END::WORD_END;
                }
                return ;
            }
            word.push_back(m_buffer[i]);
        }
        //m_read_index=0;
        int ret = read_socket();
        if(ret == -1) {
            m_end_status = END::NO;
            return ;
        } else if(ret == 0) {
            // init();
            LOG_ERROR("fd is close");
            throw std::exception();
            return ;
        }
    }
}

void HttpRequest::read_line(string &word) {
    while(true) {
        for(int i = m_read_index; i < m_end_index; i++, m_read_index++) {
            if(m_buffer[i] == '\n') {
                m_read_index += 1;
                word.pop_back();
                m_end_status = END::LINE_END;
                return ;
            }
            word.push_back(m_buffer[i]);
        }
        int ret = read_socket();
        if(ret == -1) {
            m_end_status = END::NO;
            return ;
        } else if(ret == 0) {
            m_status = STATUS::READ_HTTP_ERROR;
            LOG_ERROR("fd is close");
            return ;
        }
    }
}

bool HttpRequest::read_byte(char &c) {
    if(m_read_index == m_end_index) {
        m_read_index = 0;
        int ret = read_socket();
        if(ret == -1) {
            m_end_status = END::NO;
            return false;
        } else if(ret == 0) {
            m_status = STATUS::READ_HTTP_ERROR;
            LOG_ERROR("fd is close");
            throw std::exception();
            return false;
        }
    }
    c = m_buffer[m_read_index++];
    return true;

}





bool HttpRequest::parse() {

    if(m_status == STATUS::READ_HTTP_HEAD) { //处理请求头
        if(m_method.empty()) {
            read_word(m_string_buff1);
            if(m_end_status == END::NO)return false;
            else m_method.swap(m_string_buff1);
        }
        if(m_path.empty()) {
            read_word(m_string_buff1);
            if(m_end_status == END::NO)return false;
            else m_path.swap(m_string_buff1);
        }
        while(m_end_status == END::WORD_END || m_end_status == END::NO) {
            if(m_string_index == 0) {
                read_word(m_string_buff1);
                if(m_end_status == END::NO)return false;
                else if(m_end_status == END::LINE_END)break;
                m_string_index++;
            }
            if(m_string_index == 1) {
                read_word(m_string_buff2);
                if(m_end_status == END::NO)return false;
                m_string_index = 0;
                std::cout << m_string_buff1 << ' ' << m_string_buff2 << std::endl;
                m_values[move(m_string_buff1)] = move(m_string_buff2);
            }
        }
        if(m_protocolVersion.empty()) {
            m_protocolVersion.swap(m_string_buff1);
        }
        m_status = STATUS::READ_HTTP_LINE;
        //std::cout<<m_method<<' '<<m_path<<' '<<m_protocolVersion<<std::endl;
    }
    if(m_status == STATUS::READ_HTTP_LINE) {
        while(true) {
            // std::cout<<m_string_index<<' '<<"read line1"<<std::endl;
            if(m_string_index == 0) {
                read_word(m_string_buff1);
                if(m_end_status == END::NO)return false;
                if(m_string_buff1.empty()) {
                    if(m_method == "GET")m_status = STATUS::READ_HTTP_FINSH;
                    else m_status = STATUS::READ_HTTP_BODY;
                    break;

                }
                m_string_index++;
            }
            if(m_string_index == 1) {
                read_line(m_string_buff2);
                if(m_end_status == END::NO)return false;
                m_string_index = 0;
                //  std::cout<<m_string_buff1<<' '<<m_string_buff2<<std::endl;
                m_headers[move(m_string_buff1)] = move(m_string_buff2);
                //std::cout<<m_string_buff1<<' '<<m_string_buff2<<std::endl;
            }
        }
    }
    if(m_status == STATUS::READ_HTTP_BODY) {
        if(m_headers.count("Content-Length")) {
            int len = stoi(m_headers["Content-Length"]);
            int i = m_body.size();
            char temp;
            while(i < len) {
                //std::cout<<m_body<<std::endl;
                if(read_byte(temp)) {
                    m_body.push_back(temp);
                    i++;
                } else return false;
            }

        }
        m_status = STATUS::READ_HTTP_FINSH;
        return true;
    }
    /*switch (m_status){
    case STATUS::READ_HTTP_HEAD:  std::cout<<"READ_HTTP_HEAD"<<std::endl;break;
    case STATUS::READ_HTTP_LINE:  std::cout<<"READ_HTTP_LINE"<<std::endl;break;
    case STATUS::READ_HTTP_BODY:  std::cout<<"READ_HTTP_BODY"<<std::endl;break;
    default:  std::cout<<"READ_HTTP_FINSH"<<std::endl;break;
    }*/
    m_status = STATUS::READ_HTTP_FINSH;
    return true;
}

const string& HttpRequest::get_body() {
    return m_body;
}

const string& HttpRequest::get_header(const string &key) {
    return m_headers[key];
}

const string& HttpRequest::get_header(string &&key) {
    return m_headers[key];
}


const string& HttpRequest::get_path() {
    return m_path;
}

const string& HttpRequest::get_method() {
    return m_method;
}

const string& HttpRequest::get_protocolVersion() {
    return m_protocolVersion;
}

const string& HttpRequest::get_value(const string &key) {
    return m_values[key];
}

const string& HttpRequest::get_value(string &&key) {
    return m_values[key];
}



