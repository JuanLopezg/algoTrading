project('ALGOTRADING', 'cpp',
    version: '0.1.0',
    meson_version: '>=0.63.0',
    default_options : [
        'buildtype=debugoptimized',
        'cpp_std=c++23',
        'warning_level=3'
    ]
)

add_project_arguments(
    '-DALGOTRADING_VERSION="' + meson.project_version() + '"',
    language: 'cpp'
)

boost_dep = dependency('boost',
    modules : ['filesystem', 'program_options'],
    required : true
)

spdlog_dep = dependency('spdlog', required: true)

subdir('lib')
subdir('database')
