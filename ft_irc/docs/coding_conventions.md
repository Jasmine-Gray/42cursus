# ft_irc Coding Conventions (C++98)

## Scope
These rules apply to the full mandatory part implementation.

## Naming
- Class: `PascalCase`
- Method/function: `camelCase`
- Private member: `name_`
- Constant: `UPPER_SNAKE_CASE`

## File Organization
- Put source files under `srcs/` and public headers under `includes/`.
- Mirror module directories between `srcs/` and `includes/` when useful.
- Pair `*.hpp` + `*.cpp` per main class, for example `srcs/server/Server.cpp` and `includes/server/Server.hpp`.
- Keep declarations minimal in headers.
- Do not include unused headers.

## Makefile
- Use standard variable names: `CXX`, `CXXFLAGS`, `CPPFLAGS`, `SRCS`, `OBJS`.
- List source files explicitly in `SRCS`; do not use wildcard source discovery.
- Keep mandatory targets: `$(NAME)`, `all`, `clean`, `fclean`, `re`.
- Do not relink when sources and build rules have not changed.

## Error Handling
- Validate input before state mutation.
- Return deterministic IRC error replies.
- Use clear early returns for invalid cases.

## C++98 Constraints
- Build with `-Wall -Wextra -Werror -std=c++98 -pedantic`.
- Do not use C++11+ syntax.
- Prefer C++ standard headers over C headers where possible.

## Team Process
- Changes to this file require agreement from all 3 members.
- Update cadence: at most once per week.
