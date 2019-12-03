# 586-backend

Backend API server for issue tracking app written in C++. Create projects, workers, and tasks and assign things to things.

[Frontend Repository](https://github.com/bmcclelland/586-frontend)

[Application](http://586-frontend.s3-website-us-east-1.amazonaws.com/)

## Libraries

ORM: [ODB](https://www.codesynthesis.com/products/odb). Uses a separate compiler to generate code from annotated classes.

DI: [Boost-DI](https://boost-experimental.github.io/di)

HTTP: [CPP Rest SDK](https://github.com/Microsoft/cpprestsdk). Routing is done with a custom type-based parser.

JSON: [JSON for Modern C++](https://github.com/nlohmann/json)

JWT: [JWT-CPP](https://github.com/pokowaka/jwt-cpp)

## Building

### Requirements
- GCC 8 or 9
- Conan
- CMake 3.7
- ODB 2.5.0 beta

### Steps
- `mkdir build && cd build`
- `conan install .. --build=missing`
- `cmake ..` or `cmake -DCMAKE_BUILD_TYPE=Debug ..`
- `make`

If `CMAKE_BUILD_TYPE` is `Debug`, a separate `tests` executable will be generated, and both executables will be address-sanitized.

## Configuring

Configuration is done with json files. `mvc` expects `config.json` and `tests` expects `config.tests.json`.

Key | Example | Function
--- | --- | ---
`http_listen_addr`  | `http://0.0.0.0:8001` | 
`sqlite_db_path`    | `db.sqlite` |
`auth_issuer`       | `https://dev-ztmxpnax.auth0.com/` |
`auth_default_role` | `default` | Users other than `auth_bootstrap_admin_user` are given this role upon FIRST login. If it does not exist, it will be automatically generated with no permissions. 
`auth_admin_role`   | `administrator` | This role is given to `auth_bootstrap_admin_user` upon any login. If it does not exist, it will be automatically generated with full permissions.
`auth_bootstrap_admin_user` | `oath2\|xxxx` |
`auth_public_key` | | Auth0 public key used to verify JWTs.

Additionally, any key can be loaded from a separate file by locating the key under `load_from_files`, e.g. `"load_from_files" : { "auth_public_key": "auth.pub"}`.
