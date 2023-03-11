use ExtUtils::MakeMaker;

WriteMakefile(
    NAME => 'sakisafe',
    PREREQ_PM => {
        "Mojolicious" => 0,
        "List::MoreUtils" => 0,
        "Term::ANSIColor" => 0,
        "MIME::Types" => 0,
        "Path::Tiny" => 0
    },
    EXE_FILES => ['sakisafe.pl']
);
