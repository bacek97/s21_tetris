style: lint-license lint-verter-types lint-filenames lint-clang

lint-license:
	-@if [[ "" != `find . \( -name "*.c" -o -name "*.h" \) -exec grep -iL "<.*@.*\..*>" {} +` ]];then echo "`find . \( -name "*.c" -o -name "*.h" \) -exec grep -iL "<.*@.*\..*>" {} +` Every file should contain license boilerplate" && exit 1 ;fi

lint-verter-types:
	-@if [[ "" != `find . \( -name "*.c" -o -name "*.h" \) -exec grep -l "\b\(u\)\?int\(8\|16\|32\|64\)_t\b" {} +` ]];then echo "`find . \( -name "*.c" -o -name "*.h" \) -exec grep -il "\b\(u\)\?int\(8\|16\|32\|64\)_t\b" {} +` Verter does not support fixed-width integer types" && exit 1 ;fi

lint-filenames:
	@find . -type f \( -name "*.h" -o -name "*.c" -o -name "*.cc" \) -exec sh -c \
	'echo "{}" | rev | cut -d/ -f1 | rev | grep -Eq "^[a-z0-9_\-]+\.(h|c|cc)" || echo "-X- {} (invalid name)"' \;
	@find . -type f \( -iname "*test*.h" -o -iname "*test*.c" -o -iname "*test*.cc" \) -exec sh -c \
	'echo "{}" | rev | cut -d/ -f1 | rev | grep -Eq "^[a-z0-9_\-]+_test\.(h|c|cc)" || echo "-X- {} (invalid name *_test.*)"' \;

lint-clang:
	@find . -name "*.c" -exec clang-format  -style="{BasedOnStyle: Google, InsertBraces: true}" --verbose --Werror --ferror-limit=0 -i {} +;
	@clang-tidy -fix -header-filter=.*\.h `find . \( -name "*.c" \) -not -name "*test*"` \
    --format-style="{BasedOnStyle: Google, InsertBraces: true}" -config="{CheckOptions: [ \
	{key: readability-identifier-naming.VariableCase, value: lower_case}, \
	{key: readability-identifier-naming.FunctionCase, value: CamelCase}, \
	{key: readability-identifier-naming.TypedefCase, value: CamelCase}, \
	{key: readability-identifier-naming.ConstantCase, value: CamelCase}, \
	{key: readability-identifier-naming.EnumConstantCase, value: CamelCase}, \
	{key: readability-identifier-naming.ConstantPrefix, value: k }, \
	{key: readability-identifier-naming.EnumConstantPrefix, value: k}, \
	{key: readability-identifier-naming.MacroDefinitionCase, value: UPPER_CASE}, \
	{key: readability-identifier-naming.NamespaceCase, value: lower_case}, \
	{key: readability-identifier-naming.ConceptCase, value: CamelCase}, \
	{key: readability-identifier-naming.MemberCase, value: lower_case}, \
	{key: readability-identifier-naming.ClassMemberSuffix, value: _}, \
	\
	{key: readability-identifier-naming.FunctionCase, value: camelBack}, \
	{key: readability-identifier-naming.ConstantPrefix, value: '' }, \
	{key: readability-identifier-naming.EnumConstantPrefix, value: '' }, \
	{key: readability-identifier-naming.TypedefSuffix, value: _t}, \
	]}" \
	-checks="*,-llvmlibc*,-altera*,-android*" \
	--warnings-as-errors="*,-bugprone-easily-swappable-parameters,-readability-identifier-length" \
	 \
	-- -I"`pwd`"
	@cppcheck --enable=all --suppress=missingIncludeSystem --suppress=checkersReport --check-level=exhaustive -I. .
