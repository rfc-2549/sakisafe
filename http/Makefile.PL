use v5.36;

use ExtUtils::MakeMaker;

WriteMakefile(
    NAME => 'sakisafe',
    CONFIGURE_REQUIRES => {
        "Mojolicious" => 0,
        "List::MoreUtils" => 0,
        "Term::ANSIColor" => 0,
        "MIME::Types" => 0,
        "Path::Tiny" => 0,
        "Mojolicious::Plugin::RenderFile" => 0,
        "Mojolicious::Routes::Pattern" => 0
    },
    EXE_FILES => ['sakisafe.pl']
);
