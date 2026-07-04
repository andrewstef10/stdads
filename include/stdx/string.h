#ifndef STRING_H
#define STRING_H

#include <cstddef>
#include <stdx/iterator.h>

namespace stdx {

/**
 * @brief The default capacity of a string in bytes
 *
 * Used by the string class to determine how many byes to allocate on the stack by default for SSO (Small string
 * Optimization)
 */
const static std::size_t DEFAULT_STRING_CAPACITY_BYTES = 24;

/**
 * @brief Searches within the first num bytes of the block of memory pointed by ptr for the first occurrence of value
 * (interpreted as an unsigned char), and returns a pointer to it.
 *
 * Both value and each of the bytes checked on the the ptr array are interpreted as unsigned char for the comparison.
 * Passing ptr as nullptr results in undefined behavior.
 *
 * @param ptr Pointer to the block of memory to be searched
 * @param value Value to be located. The value is passed as an int, but the function performs a byte per byte search
 * using the unsigned char conversion of this value.
 * @param num Number of bytes to be analyzed
 * @return A pointer to the first occurrence of value in the block of memory pointed by ptr.
 *         If the value is not found, the function returns a null pointer.
 */
const void* memchr(const void* ptr, int value, std::size_t num);

/**
 * @brief Compare two blocks of memory
 *
 * Compares the first num bytes of the block of memory pointed by ptr1 to the first num bytes pointed by ptr2.
 * Returns zero if they all match or a value different from zero representing which is greater if they do not.
 * Passing ptr1 or ptr2 as nullptr results in undefined behavior.
 *
 * @param ptr1 Pointer to a memory block
 * @param ptr2 Pointer to a memory block
 * @param num Number of bytes to compare
 * @return Zero if both memory blocks are equal.
 *         Negative if the first byte that does not match in both memory blocks has a lower value in ptr1 than in ptr2
 * (if evaluated as unsigned char values). Positive if the first byte that does not match in both memory blocks has a
 * greater value in ptr1 than in ptr2 (if evaluated as unsigned char values).
 */
int memcmp(const void* ptr1, const void* ptr2, std::size_t num);

/**
 * @brief Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to
 * by destination.
 *
 * The underlying type of the objects pointed to by both the source and destination pointers are irrelevant for this
 * function; The result is a binary copy of the data. The function does not check for any terminating null character in
 * source - it always copies exactly num bytes. To avoid overflows, the size of the arrays pointed to by both the
 * destination and source parameters, shall be at least num bytes, and should not overlap (for overlapping memory
 * blocks, memmove is a safer approach). Passing destination or source as nullptr results in undefined behavior.
 *
 * @param destination Pointer to the destination array where the content is to be copied, type-casted to a pointer of
 * type void*.
 * @param source Pointer to the source of data to be copied, type-casted to a pointer of type const void*.
 * @param num Number of bytes to copy.
 * @return destination is returned.
 */
void* memcpy(void* destination, const void* source, std::size_t num);

/**
 * @brief Sets the first num bytes of the block of memory pointed by ptr to the specified value (interpreted as an
 * unsigned char).
 *
 * Passing ptr as nullptr results in undefined behavior.
 *
 * @param ptr Pointer to the block of memory to fill.
 * @param value Value to be set. The value is passed as an int, but the function fills the block of memory using the
 * unsigned char conversion of this value.
 * @param num Number of bytes to be set to the value.
 * @return ptr is returned.
 */
void* memset(void* ptr, int value, std::size_t num);

/**
 * @brief Concatenate strings
 *
 * Appends a copy of the source string to the destination string.
 * The terminating null character in destination is overwritten by the first character of source, and a null-character
 * is included at the end of the new string formed by the concatenation of both in destination. destination and source
 * shall not overlap.
 *
 * @param destination Pointer to the destination array, which should contain a C string, and be large enough to contain
 * the concatenated resulting string.
 * @param source C string to be appended. This should not overlap destination.
 * @return destination is returned.
 */
char* strcat(char* destination, const char* source);

/**
 * @brief Locate first occurrence of character in string
 *
 * Returns a pointer to the first occurrence of character in the C string str.
 * The terminating null-character is considered part of the C string. Therefore, it can also be located in order to
 * retrieve a pointer to the end of a string.
 *
 * @param str C string.
 * @param character Character to be located. It is passed as its int promotion, but it is internally converted back to
 * char for the comparison.
 * @return A pointer to the first occurrence of character in str. If the character is not found, the function returns a
 * null pointer.
 */
const char* strchr(const char* str, int character);

/**
 * @brief Compares the C string str1 to the C string str2.
 *
 * This function starts comparing the first character of each string. If they are equal to each other, it continues with
 * the following pairs until the characters differ or until a terminating null-character is reached. This function
 * performs a binary comparison of the characters. Passing str1 or str2 as nullptr results in undefined behavior.
 *
 * @param str1 C string to be compared.
 * @param str2 C string to be compared.
 * @return Zero if the contents of both strings are equal.
 *         Negative if the first character that does not match has a lower value in ptr1 than in ptr2.
 *         Positive if the first character that does not match has a greater value in ptr1 than in ptr2
 */
int strcmp(const char* str1, const char* str2);

/**
 * @brief Copies the C string pointed by source into the array pointed by destination, including the terminating null
 * character (and stopping at that point).
 *
 * To avoid overflows, the size of the array pointed by destination shall be long enough to contain the same C string as
 * source (including the terminating null character), and should not overlap in memory with source. Passing destination
 * or source as nullptr results in undefined behavior.
 *
 * @param destination Pointer to the destination array where the content is to be copied.
 * @param source C string to be copied.
 * @return destination is returned.
 */
char* strcpy(char* destination, const char* source);

/**
 * @brief Returns the length of the C string str.
 *
 * The length of a C string is determined by the terminating null-character:
 * A C string is as long as the number of characters between the beginning of the string and the terminating null
 * character (without including the terminating null character itself). Passing str as nullptr results in undefined
 * behavior.
 *
 * @param str C string.
 * @return The length of str.
 */
std::size_t strlen(const char* str);

/// @brief Determines whether a null-terminated C-string contains a character.
/// @details Performs a linear search through the string.
///
/// Time Complexity:
///     - Time (worst case): O(n), where n is the length of the string `str`.
///     - Space: O(1)
///
/// @param str Pointer to the null-terminated string to search.
/// @param c Character to search for within the string.
/// @return true if the character exists in the string; otherwise false.
/// @warning Passing a null pointer results in undefined behavior.
bool contains(const char* str, char c);

/// @brief Determines whether a null-terminated C-string contains any character from another null-terminated C-string.
/// @details Performs a linear search through both strings.
///
/// Time Complexity:
///     - Time (worst case): O(n * m), where n is the length of `str` and m is the
///     - Space: O(1)
///
/// @param str Pointer to the null-terminated string to search.
/// @param chars Pointer to the null-terminated string containing the set of
///              characters to search for.
/// @return true if any character from chars exists in `str`; otherwise false.
/// @warning Passing a null pointer results in undefined behavior.
bool contains_any(const char* str, const char* chars);

/// @brief Removes leading trim characters from a null-terminated C-string.
/// @details Does not allocate memory or modify characters.
///
/// Time Complexity:
///     - Time (worst case): O(n * m), where n is the length of `str` and m is the length of `trimChars`.
///     - Space: O(1)
///
/// @param str Pointer to the null-terminated string to trim.
/// @param trimChars Pointer to a null-terminated string containing the set of
///                  characters to remove from the beginning of the string `str`.
///                  Defaults to a space character.
/// @return Pointer to the first character in `str` that is not contained in `trimChars`.
/// @warning Passing a null pointer results in undefined behavior.
const char* trim_front(const char* str, const char* trimChars = " ");
char* trim_front(char* str, const char* trimChars = " ");

/// @brief Removes trailing trim characters from a null-terminated C-string.
/// @details Does not allocate memory. Replaces trailing characters found in `trimChars`
///          with the null terminator (`'\0'`), modifying the original buffer.
///
/// Time Complexity:
///     - Time (worst case): O(n * m), where n is the length of `str` and m is the length of `trimChars`.
///     - Space: O(1)
///
/// @param str Pointer to the null-terminated string to trim.
/// @param len Length of the string excluding the null terminator.
/// @param trimChars Pointer to a null-terminated string containing the set of
///                  characters to remove from the back of the string `str`.
///                  Defaults to a space character.
/// @return Pointer to `str`.
/// @warning Passing a null pointer results in undefined behavior.
/// @warning `len` must match the actual string length.
char* trim_back(char* str, std::size_t len, const char* trimChars = " ");

/// @brief Removes leading and trailing trim characters from a null-terminated C-string.
/// @details Does not allocate memory. Replaces trailing characters found in `trimChars`
///          with the null terminator (`'\0'`), modifying the original buffer.
///
/// Time Complexity:
///     - Time (worst case): O(n * m), where n is the length of `str` and m is the length of `trimChars`.
///     - Space: O(1)
///
/// @param str Pointer to the null-terminated string to trim.
/// @param len Length of the string excluding the null terminator.
/// @param trimChars Pointer to a null-terminated string containing the set of
///                  characters to remove from the front and back of the string `str`.
///                  Defaults to a space character.
/// @return Pointer to `str`.
/// @warning Passing a null pointer results in undefined behavior.
/// @warning `len` must match the actual string length.
char* trim(char* str, std::size_t len, const char* trimChars = " ");

/**
 * @brief string class
 *
 * The templated parameter N represents the initial capacity of the string allocated on the stack.
 * If the string outgrows its initial capacity, dynamic memory allocation will be used to grow the string.
 *
 * @param N The initial capacity of this string allocated on the stack.
 */
template <std::size_t N = DEFAULT_STRING_CAPACITY_BYTES>
class string {
public:
  using iterator = char*;
  using const_iterator = const char*;
  using reverse_iterator = stdx::reverse_iterator<iterator>;
  using const_reverse_iterator = stdx::reverse_iterator<const_iterator>;

  /**
   * @brief Default constructor.
   *
   * Creates an empty string.
   */
  string();

  /**
   * @brief Copy constructor.
   *
   * @param other string to copy.
   */
  string(const string& other);
  template <std::size_t M>
  string(const string<M>& other);

  /**
   * @brief C string constructor.
   *
   * @param cstr C string.
   */
  string(const char* cstr);

  /**
   * @brief Character constructor.
   *
   * @param c A character.
   */
  string(char c);

  /**
   * @brief Destructor.
   */
  ~string();

  /// @brief Move constructor
  /// @param
  string(string&&) = default;

  /**
   * @brief Assigns a new value to the string, replacing its current contents.
   *
   * @param other string object to copy if different from *this.
   * @return Reference to this string object
   */
  string& operator=(const string& other) {
    assign(other.c_str(), other.size());
    return *this;
  }
  template <std::size_t M>
  string& operator=(const string<M>& other) {
    assign(other.c_str(), other.size());
    return *this;
  }

  /**
   * @brief Assigns a new value to the string, replacing its current contents.
   *
   * @param cstr Pointer to a null-terminated sequence of characters. The sequence is copied as the new value for the
   * string.
   * @return Reference to this string object
   */
  string& operator=(const char* cstr) {
    assign(cstr, strlen(cstr));
    return *this;
  }

  /**
   * @brief Assigns a new value to the string, replacing its current contents.
   *
   * @param c A character. The string value is set to a single copy of this character.
   * @return Reference to this string object
   */
  string& operator=(char c);

  /// @brief Move assignment operator
  /// @param
  /// @return Reference to this string object
  string& operator=(string&&) = default;

  //////////////////////// Capacity ////////////////////////

  /**
   * @brief Returns the length of the string, in terms of bytes (excluding the null terminator).
   *
   * This is the number of actual bytes that conform the contents of the string, which is not necessarily equal to its
   * storage capacity.
   *
   * @return The number of bytes in the string.
   */
  std::size_t size() const { return m_size; }

  /**
   * @brief Return size of allocated storage
   *
   * Returns the size of the storage space currently allocated for the string, expressed in terms of bytes (excluding
   * the null terminator). This capacity is not necessarily equal to the string length. It can be equal or greater,
   * with the extra space allowing the object to optimize its operations when new characters are added to the string.
   *
   * @return The size of the storage capacity currently allocated for the string.
   */
  std::size_t capacity() const { return m_capacity; }

  /**
   * @brief Returns whether the string is empty (i.e. whether its length is 0).
   *
   * @return true if the string length is 0, false otherwise.
   */
  bool empty() const { return m_size == 0; }

  //////////////////////// Element Access ////////////////////////

  /**
   * @brief Get contents as a C string.
   *
   * Returns a pointer to an array that contains a null-terminated sequence of characters (i.e., a C-string)
   * representing the current value of the string object.
   *
   * @return A pointer to the c-string representation of the string object's value.
   */
  const char* c_str() const { return get_data(); }

  /**
   * @brief Returns a reference to the character at specified location pos if pos < size(), or if pos == size()
   *
   * If pos > size(), the behavior is undefined. Bounds checking is not performed.
   *
   * @param pos Position of the character to return
   * @return Reference to char at position pos
   */
  char& operator[](std::size_t pos) { return get_data()[pos]; }
  const char& operator[](std::size_t pos) const { return get_data()[pos]; }

  //////////////////////// Iterators ////////////////////////

  /// @brief Returns a contiguous iterator to the first element of *this.
  /// If *this is empty, the returned iterator will be equal to end().
  /// @return contiguous iterator to the first element.
  iterator begin() { return iterator(get_data()); }
  const_iterator begin() const { return const_iterator(get_data()); }
  const_iterator cbegin() const { return const_iterator(get_data()); }

  /// @brief Returns a contiguous iterator past the last element of *this.
  /// This returned iterator only acts as a sentinel. It is not guaranteed to be dereferenceable.
  /// @return contiguous iterator past the last element.
  iterator end() { return iterator(get_data() + m_size); }
  const_iterator end() const { return const_iterator(get_data() + m_size); }
  const_iterator cend() const { return const_iterator(get_data() + m_size); }

  /// @brief Returns a reverse contiguous iterator to the first element of the reversed *this. It corresponds to the
  /// last element of the non-reversed *this. If *this is empty, the returned iterator will be equal to end().
  /// @return Reverse contiguous iterator to the first element.
  reverse_iterator rbegin() { return reverse_iterator(get_data() + m_size); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(get_data() + m_size); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(get_data() + m_size); }

  /// @brief Returns a reverse contiguous iterator past the last element of the reversed *this. It corresponds to the
  /// element preceding the first element of the non-reversed *this. This returned iterator only acts as a sentinel.
  /// It is not guaranteed to be dereferenceable.
  /// @return Reverse contiguous iterator to the element following the last element.
  reverse_iterator rend() { return reverse_iterator(get_data()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(get_data()); }
  const_reverse_iterator crend() const { return const_reverse_iterator(get_data()); }

  //////////////////////// Modifiers ////////////////////////

  /**
   * @brief Addition Assignment Operator.
   *
   * Extends the string by appending additional characters at the end of its current value.
   *
   * @param other A string object, whose value is copied at the end.
   * @return Reference to this string object
   */
  string& operator+=(const string<N>& other) {
    append(other.c_str(), other.size());
    return *this;
  }
  template <std::size_t M>
  string& operator+=(const string<M>& other) {
    append(other.c_str(), other.size());
    return *this;
  }

  /**
   * @brief Addition Assignment Operator.
   *
   * Extends the string by appending additional characters at the end of its current value.
   *
   * @param cstr Pointer to a null-terminated sequence of characters. The sequence is copied at the end of the string.
   * @return Reference to this string object
   */
  string& operator+=(const char* cstr) {
    append(cstr, strlen(cstr));
    return *this;
  }

  /**
   * @brief Addition Assignment Operator.
   *
   * Extends the string by appending additional character at the end of its current value.
   *
   * @param c A character, which is appended to the current value of the string.
   * @return Reference to this string object
   */
  string& operator+=(char c);

  /**
   * @brief Erases the contents of the string, which becomes an empty string (with a length of 0 characters).
   */
  void clear();

private:
  std::size_t m_size;     // length of the string excluding the null terminator
  std::size_t m_capacity; // number of bytes currently allocated for the string excluding the null terminator

  union string_data {
    // initial stack allocated memory for this string
    char stack_buffer[N + 1];

    // pointer to strings memory after capacity has grown
    char* heap_ptr;
  } m_data;

  /**
   * @brief Helper to get a pointer to the correct buffer (stack or heap)
   *
   * @returns Pointer to the string's character buffer
   */
  char* get_data() { return m_capacity > N ? m_data.heap_ptr : m_data.stack_buffer; }
  const char* get_data() const { return m_capacity > N ? m_data.heap_ptr : m_data.stack_buffer; }

  /**
   * @brief Delete any dynamically allocated memory for this class
   */
  void deallocate();

  /**
   * @brief Calculates the new string capacity given the minimum size required.
   * Capacity will be repeatedly doubled until minimumSize is met or exceeded.
   *
   * @param minimumSize
   * @returns The new capacity in bytes that should be allocated to fit minimumSize bytes of data.
   */
  size_t calculate_new_capacity(std::size_t minimumSize) const;

  /**
   * @brief Set this string equal to a C string
   *
   * This Function grows this string's capacity if the provided cstr is larger than the current capacity.
   *
   * @param cstr C string to assign to this
   * @param cstrLength The length of the cstr
   * @returns Reference to this string object
   */
  void assign(const char* cstr, std::size_t cstrLength);

  /**
   * @brief Append a C string to the end of this string
   *
   * This Function grows this string's capacity if the provided cstr is larger than the current capacity.
   *
   * @param cstr C string to append to this
   * @param cstrLength The length of the cstr
   * @returns Reference to this string object
   */
  void append(const char* cstr, std::size_t cstrLength);
};

////////////////////////string Non Members ////////////////////////

/**
 * @brief string Equality Operator
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return True if lhs and rhs are equal, false otherwise.
 */
template <std::size_t N>
bool operator==(const string<N>& lhs, const string<N>& rhs);
template <std::size_t N, std::size_t M>
bool operator==(const string<N>& lhs, const string<M>& rhs);

/**
 * @brief C string Equality Operator
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return True if lhs and rhs are equal, false otherwise.
 */
template <std::size_t N>
bool operator==(const string<N>& lhs, const char* rhs);
template <std::size_t N>
inline bool operator==(const char* lhs, const string<N>& rhs) {
  return rhs == lhs;
}

/**
 * @brief Character Equality Operator
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return True if lhs and rhs are equal, false otherwise.
 */
template <std::size_t N>
bool operator==(const string<N>& lhs, char rhs);
template <std::size_t N>
inline bool operator==(char lhs, const string<N>& rhs) {
  return rhs == lhs;
}

/**
 * @brief string Inequality Operator
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return True if lhs and rhs are equal, false otherwise.
 */
template <std::size_t N>
inline bool operator!=(const string<N>& lhs, const string<N>& rhs) {
  return !(lhs == rhs);
}
template <std::size_t N, std::size_t M>
inline bool operator!=(const string<N>& lhs, const string<M>& rhs) {
  return !(lhs == rhs);
}

/**
 * @brief C string Inequality Operator
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return True if lhs and rhs are equal, false otherwise.
 */
template <std::size_t N>
inline bool operator!=(const string<N>& lhs, const char* rhs) {
  return !(lhs == rhs);
}
template <std::size_t N>
inline bool operator!=(const char* lhs, const string<N>& rhs) {
  return !(rhs == lhs);
}

/**
 * @brief Character Inequality Operator
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return True if lhs and rhs are equal, false otherwise.
 */
template <std::size_t N>
inline bool operator!=(const string<N>& lhs, char rhs) {
  return !(lhs == rhs);
}
template <std::size_t N>
inline bool operator!=(char lhs, const string<N>& rhs) {
  return !(rhs == lhs);
}

//////////////////////// Inline string Implementation ////////////////////////

template <std::size_t N>
inline string<N>::string() : m_size(0), m_capacity(N), m_data{{'\0'}} {}

template <std::size_t N>
inline string<N>::string(const string<N>& other) : m_size(0), m_capacity(N), m_data{{'\0'}} {
  assign(other.c_str(), other.size());
}

template <std::size_t N>
template <std::size_t M>
inline string<N>::string(const string<M>& other) : m_size(0), m_capacity(N), m_data{{'\0'}} {
  assign(other.c_str(), other.size());
}

template <std::size_t N>
inline string<N>::string(const char* cstr) : m_size(0), m_capacity(N), m_data{{'\0'}} {
  assign(cstr, strlen(cstr));
}

template <std::size_t N>
inline string<N>::string(char c) : m_size(0), m_capacity(N), m_data{{'\0'}} {
  char buff[] = {c, '\0'};
  assign(buff, 1);
}

template <std::size_t N>
inline string<N>::~string() {
  deallocate();
}

template <std::size_t N>
inline string<N>& string<N>::operator=(char c) {
  char buff[] = {c, '\0'};
  assign(buff, 1);
  return *this;
}

template <std::size_t N>
inline string<N>& string<N>::operator+=(char c) {
  char buff[] = {c, '\0'};
  append(buff, 1);
  return *this;
}

template <std::size_t N>
inline void string<N>::clear() {
  m_size = 0;
  get_data()[0] = '\0';
}

template <std::size_t N>
inline void string<N>::deallocate() {
  if (m_capacity > N) {
    delete[] m_data.heap_ptr;
    m_data.heap_ptr = 0;
  }
}

template <std::size_t N>
inline size_t string<N>::calculate_new_capacity(std::size_t minimumSize) const {
  size_t newCapacity = m_capacity;
  while (newCapacity < minimumSize) {
    if (newCapacity == 0) {
      newCapacity = 1;
    } else {
      newCapacity *= 2;
    }
  }
  return newCapacity;
}

template <std::size_t N>
inline void string<N>::assign(const char* cstr, std::size_t cstrLength) {
  if (cstrLength > m_capacity) {
    // need to allocate more memory
    deallocate();
    m_capacity = calculate_new_capacity(cstrLength);
    m_data.heap_ptr = new char[m_capacity + 1];
  }

  m_size = cstrLength;
  char* data = get_data();
  memcpy(data, cstr, cstrLength); // assign all but the null term
  data[m_size] = '\0';
}

template <std::size_t N>
inline void string<N>::append(const char* cstr, std::size_t cstrLength) {
  char* data = get_data();
  if (m_size + cstrLength > m_capacity) {
    // Need to allocate more memory.
    size_t newCapacity = calculate_new_capacity(m_size + cstrLength);
    char* tempData = new char[newCapacity + 1];
    memcpy(tempData, data, m_size); // copy current string. No need to copy null term (we are appending)

    deallocate();
    m_capacity = newCapacity;
    m_data.heap_ptr = tempData;
    data = m_data.heap_ptr;
  }

  memcpy(data + m_size, cstr, cstrLength); // append other string excluding the null term
  m_size += cstrLength;
  data[m_size] = '\0';
}

template <std::size_t N>
inline bool operator==(const string<N>& lhs, const string<N>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  return memcmp(lhs.c_str(), rhs.c_str(), lhs.size() + 1) == 0;
}

template <std::size_t N, std::size_t M>
inline bool operator==(const string<N>& lhs, const string<M>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  return memcmp(lhs.c_str(), rhs.c_str(), lhs.size() + 1) == 0;
}

template <std::size_t N>
inline bool operator==(const string<N>& lhs, const char* rhs) {
  return memcmp(lhs.c_str(), rhs, lhs.size() + 1) == 0;
}

template <std::size_t N>
inline bool operator==(const string<N>& lhs, char rhs) {
  return lhs.size() == 1 && lhs.c_str()[0] == rhs;
}
} // namespace stdx

#endif
