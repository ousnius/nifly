-- set project
set_project("nifly")
set_languages("c++17")
set_license("GPL-3.0")

-- define options
option("tests", { default = false, description = "Enable tests" })

-- require packages
add_requires("half", "miniball")

if has_config("tests") then
    add_requires("catch2 v2.13.10")
end

-- define targets
target("nifly", function()
    set_kind("static")

    -- bind package dependencies
    add_packages("half", "miniball", { public = true })

    -- add all source files
    add_files("src/**.cpp")

    -- add all header files
    add_includedirs("include", { public = true })
    add_headerfiles("include/(**.hpp)", { prefixdir = "nifly" })

    -- add flags
    add_cxxflags("cl::/Zc:inline", "cl::/bigobj")
end)

if has_config("tests") then
    target("nifly-tests", function()
        -- add target dependencies
        add_deps("nifly")

        -- bind package dependencies
        add_packages("catch2")

        -- add all source files, except TestNifFileOptional
        add_files("tests/*.cpp|TestNifFileOptional.cpp")

        -- add all header files
        add_includedirs("tests")

        -- set run directory
        set_rundir("tests")
    end)
end
