#include "core/RString.h"
#include <algorithm>
#include <cctype>
#include <fmt/format.h>

namespace REngine {
    // ===== Core Constructors =====
    RString::RString() noexcept {
        sso_buffer[0] = '\0';  // Empty SSO string
        m_length = 0;
    }

    RString::RString(const char* str) : RString(str, str ? std::strlen(str) : 0) {}

    RString::RString(const char* str, size_t length) {
        if (length <= SSO_CAPACITY) {
            // SSO storage
            std::memcpy(sso_buffer, str, length);
            sso_buffer[length] = '\0';
            m_length = length;
        } else {
            // Heap allocation
            Reallocate(length + 1);  // +1 for null terminator
            std::memcpy(heap_ptr, str, length);
            heap_ptr[length] = '\0';
            m_length = length;
        }
    }

    // ===== Destructor =====
    RString::~RString() {
        if (!UsingSSO()) {
            delete[] heap_ptr;
            heap_ptr = nullptr;
            heap_capacity = 0;
        }
        m_length = 0;
    }

    // ===== Copy Assignment =====
    RString& RString::operator=(const RString& other) {
        if (this != &other) {
            // Destroy current content
            if (!UsingSSO() && heap_ptr) {
                delete[] heap_ptr;
            }

            m_length = other.m_length;

            if (other.UsingSSO()) {
                // Copy SSO data
                std::memcpy(sso_buffer, other.sso_buffer, SSO_CAPACITY + 1);
            } else {
                // Allocate and copy heap data
                heap_capacity = other.heap_capacity;
                heap_ptr = new char[heap_capacity];
                std::memcpy(heap_ptr, other.heap_ptr, m_length + 1);
            }
        }
        return *this;
    }

    // ===== Private Helpers =====
    void RString::Reallocate(size_t new_capacity) {
        char* new_data = new char[new_capacity];
        if (m_length > 0) {
            // Copy existing data (whether from SSO or heap)
            const char* src = UsingSSO() ? sso_buffer : heap_ptr;
            std::memcpy(new_data, src, m_length + 1);
        }

        // Clean up old allocation if needed
        if (!UsingSSO() && heap_ptr) {
            delete[] heap_ptr;
        }

        // Update to new allocation
        heap_ptr = new_data;
        heap_capacity = new_capacity;
    }

    bool RString::UsingSSO() const noexcept {
        return m_length <= SSO_CAPACITY;
    }


    // ===== Core Functions =====
    RString::RString(const RString& other) : m_length(other.m_length) {
        if (other.UsingSSO()) {
            std::memcpy(sso_buffer, other.sso_buffer, SSO_CAPACITY + 1);
        } else {
            Reallocate(other.heap_capacity);
            std::memcpy(heap_ptr, other.heap_ptr, m_length + 1);
        }
    }

    RString::RString(RString&& other) noexcept : m_length(other.m_length) {
        if (other.UsingSSO()) {
            std::memcpy(sso_buffer, other.sso_buffer, m_length + 1);
        } else {
            heap_ptr = other.heap_ptr;
            heap_capacity = other.heap_capacity;
        }
        other.m_length = 0;
        other.sso_buffer[0] = '\0';
    }

    // ===== Assignment Operators =====
    RString& RString::operator=(const char* str) {
        size_t len = str ? std::strlen(str) : 0;
        if (len <= SSO_CAPACITY) {
            Destroy();
            std::memcpy(sso_buffer, str, len);
            sso_buffer[len] = '\0';
            m_length = len;
        } else {
            Reallocate(len + 1);
            std::memcpy(heap_ptr, str, len + 1);
            m_length = len;
        }
        return *this;
    }

    RString& RString::operator=(RString&& other) noexcept {
        if (this != &other) {
            Destroy();
            m_length = other.m_length;
            if (other.UsingSSO()) {
                std::memcpy(sso_buffer, other.sso_buffer, m_length + 1);
            } else {
                heap_ptr = other.heap_ptr;
                heap_capacity = other.heap_capacity;
            }
            other.m_length = 0;
            other.sso_buffer[0] = '\0';
        }
        return *this;
    }

    // ===== String Operations =====
    RString RString::operator+(const RString& rhs) const {
        RString result;
        result.Reserve(m_length + rhs.m_length + 1);
        std::memcpy(result.UsingSSO() ? result.sso_buffer : result.heap_ptr,
                   UsingSSO() ? sso_buffer : heap_ptr, m_length);
        std::memcpy((result.UsingSSO() ? result.sso_buffer : result.heap_ptr) + m_length,
                   rhs.UsingSSO() ? rhs.sso_buffer : rhs.heap_ptr, rhs.m_length + 1);
        result.m_length = m_length + rhs.m_length;
        return result;
    }

    RString& RString::operator+=(const RString& rhs) {
        const size_t new_length = m_length + rhs.m_length;
        Reserve(new_length + 1);
        char* dest = UsingSSO() ? sso_buffer : heap_ptr;
        const char* src = rhs.UsingSSO() ? rhs.sso_buffer : rhs.heap_ptr;
        std::memcpy(dest + m_length, src, rhs.m_length + 1);
        m_length = new_length;
        return *this;
    }

    size_t RString::Length() const noexcept {
        return m_length;
    }

    const char* RString::c_str() const noexcept {
        return UsingSSO() ? sso_buffer : heap_ptr;
    }

    // ===== Memory Management =====
    void RString::Reserve(size_t capacity) {
        if (capacity <= (UsingSSO() ? SSO_CAPACITY : heap_capacity)) return;
        Reallocate(capacity);
    }

    void RString::ShrinkToFit() {
        if (UsingSSO() || m_length >= heap_capacity) return;
        if (m_length <= SSO_CAPACITY) {
            char temp[SSO_CAPACITY + 1];
            std::memcpy(temp, heap_ptr, m_length + 1);
            delete[] heap_ptr;
            std::memcpy(sso_buffer, temp, m_length + 1);
        } else {
            char* new_ptr = new char[m_length + 1];
            std::memcpy(new_ptr, heap_ptr, m_length + 1);
            delete[] heap_ptr;
            heap_ptr = new_ptr;
            heap_capacity = m_length;
        }
    }

    // ===== Search Methods =====
    size_t RString::Find(char ch, size_t start_pos) const {
        int npos = -1;
        if (start_pos >= m_length) return npos;
        const char* data = UsingSSO() ? sso_buffer : heap_ptr;
        const char* found = static_cast<const char*>(std::memchr(data + start_pos, ch, m_length - start_pos));
        return found ? (found - data) : npos;
    }

    size_t RString::Find(const char* substr, size_t start_pos) const {
        int npos = -1;
        if (start_pos >= m_length) return npos;
        const char* data = UsingSSO() ? sso_buffer : heap_ptr;
        const char* found = std::strstr(data + start_pos, substr);
        return found ? (found - data) : npos;
    }

    size_t RString::FindLast(char ch) const {
        int npos = -1;
        const char* data = UsingSSO() ? sso_buffer : heap_ptr;
        for (size_t i = m_length; i-- > 0;) {
            if (data[i] == ch) return i;
        }
        return npos;
    }

    // ===== Modification Methods =====
    RString& RString::ToUpper() {
        char* data = UsingSSO() ? sso_buffer : heap_ptr;
        for (size_t i = 0; i < m_length; ++i) {
            data[i] = static_cast<char>(std::toupper(data[i]));
        }
        return *this;
    }

    RString& RString::ToLower() {
        char* data = UsingSSO() ? sso_buffer : heap_ptr;
        for (size_t i = 0; i < m_length; ++i) {
            data[i] = static_cast<char>(std::tolower(data[i]));
        }
        return *this;
    }

    RString& RString::Trim() {
        if (m_length == 0) return *this;

        char* data = UsingSSO() ? sso_buffer : heap_ptr;
        size_t start = 0;
        size_t end = m_length - 1;

        while (start <= end && std::isspace(data[start])) start++;
        while (end >= start && std::isspace(data[end])) end--;

        if (start > 0 || end < m_length - 1) {
            size_t new_length = end - start + 1;
            std::memmove(data, data + start, new_length);
            data[new_length] = '\0';
            m_length = new_length;
        }

        return *this;
    }

    // ===== Query Methods =====
    bool RString::StartsWith(const char* prefix) const {
        size_t prefix_len = std::strlen(prefix);
        if (prefix_len > m_length) return false;
        const char* data = UsingSSO() ? sso_buffer : heap_ptr;
        return std::strncmp(data, prefix, prefix_len) == 0;
    }

    bool RString::EndsWith(const char* suffix) const {
        size_t suffix_len = std::strlen(suffix);
        if (suffix_len > m_length) return false;
        const char* data = UsingSSO() ? sso_buffer : heap_ptr;
        return std::strncmp(data + m_length - suffix_len, suffix, suffix_len) == 0;
    }

    bool RString::Empty() const noexcept {
        return m_length == 0;
    }

    // ===== Private Helpers =====
    void RString::Destroy() noexcept {
        if (!UsingSSO() && heap_ptr) {
            delete[] heap_ptr;
            heap_ptr = nullptr;
            heap_capacity = 0;
        }
        m_length = 0;
    }

    // ===== Formatting =====
    // Helper function to convert RString to string_view
    template <typename... Args>
    RString RString::Format(fmt::format_string<Args...> fmt, Args&&... args) {
        fmt::memory_buffer buf;
        fmt::format_to(std::back_inserter(buf), fmt, std::forward<Args>(args)...);
        return RString(buf.data(), buf.size());
    }

    // Explicit template instantiations
    template RString RString::Format<int>(fmt::format_string<int>, int&&);
    template RString RString::Format<float>(fmt::format_string<float>, float&&);
    template RString RString::Format<const char*>(fmt::format_string<const char*>, const char*&&);
    template RString RString::Format<std::string_view>(fmt::format_string<std::string_view>, std::string_view&&);


}