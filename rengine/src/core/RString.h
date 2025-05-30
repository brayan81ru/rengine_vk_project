#pragma once
#include <cstring>
#include <fmt/core.h>

namespace REngine {
    class RString {
    public:
        // ===== Core Interface (Your Original Methods) =====
        RString() noexcept;
        RString(const char* str);  // Keep implicit conversion
        RString(const char* str, size_t length);
        RString(const RString& other);
        RString(RString&& other) noexcept;
        ~RString();

        // Assignment
        RString& operator=(const char* str);
        RString& operator=(const RString& other);
        RString& operator=(RString&& other) noexcept;

        // String Operations
        RString operator+(const RString& rhs) const;
        RString& operator+=(const RString& rhs);
        [[nodiscard]] size_t Length() const noexcept;
        [[nodiscard]] const char* c_str() const noexcept;

        // Formatting
        template <typename... Args>
        static RString Format(fmt::format_string<Args...> fmt, Args&&... args);

        // Memory Management
        void Reserve(size_t capacity);
        void ShrinkToFit();

        // ===== New Utility Methods =====
        // Search
        [[nodiscard]] size_t Find(char ch, size_t start_pos = 0) const;
        size_t Find(const char* substr, size_t start_pos = 0) const;
        [[nodiscard]] size_t FindLast(char ch) const;

        // Modification
        RString& ToUpper();
        RString& ToLower();
        RString& Trim();

        // Query
        bool StartsWith(const char* prefix) const;
        bool EndsWith(const char* suffix) const;
        [[nodiscard]] bool Empty() const noexcept;

    private:
        // SSO Buffer (24 bytes on x64)
        static constexpr size_t SSO_CAPACITY = sizeof(void*) * 3 - 1;
        union {
            char sso_buffer[SSO_CAPACITY + 1];  // +1 for null terminator
            struct {
                char* heap_ptr;
                size_t heap_capacity;
            };
        };
        size_t m_length = 0;

        void Reallocate(size_t new_capacity);
        bool UsingSSO() const noexcept;
        void Destroy() noexcept;
    };

    // Non-member operator
    RString operator+(const char* lhs, const RString& rhs);
}