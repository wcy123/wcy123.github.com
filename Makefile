TEMPLATE = ./template.tmp
CSS = writ.min.css

# Rule for converting github flavored markdown to html5
MARKDOWN := pandoc --filter pandoc-include --from markdown --template $(TEMPLATE) --mathjax -c $(CSS)

DEPLOY = deploy
# Deploy directory.
# Excluded from source search. Prepended to all output files
DEPLOY_DIRECTORY = ./$(DEPLOY)/

# Source control directory, also excluded from source search
SRC_CTL = ./.git%

# All markdown files. Recursive search with `find`
ALL_BLOG = $(shell find blog -type f -and -iname '*.md')
ALL_MD = $(ALL_BLOG) index.md
ALL_MP = $(shell find blog -type f -and -iname '*.mp')
# For all known markdown files: change md extension to html and prepend the
# deploy directory.
HTML_FROM_MD := $(patsubst %.md, $(DEPLOY_DIRECTORY)%.html,$(ALL_MD))
PNG_FROM_MP := $(patsubst %.mp, $(DEPLOY_DIRECTORY)%.png,$(ALL_MP))

ALL_SRC_JEPG := $(shell find blog -type f -and -iname '*.jpg')
ALL_DST_JEPG := $(patsubst %.jpg, $(DEPLOY_DIRECTORY)%.jpg,$(ALL_SRC_JEPG))


OTHER += $(DEPLOY_DIRECTORY)writ.min.css $(ALL_DST_JEPG)


# First recipe is default. Nothing to do except dependency on all html files.
bake: $(HTML_FROM_MD) $(OTHER) $(PNG_FROM_MP)

clean:
	rm -rf $(DEPLOY_DIRECTORY) `find . -iname '*.1' -or -iname 'mptextmp.tmp'`

# Recipe for html files in the deploy directory for a corresponding markdown
# file

.PHONY:index.md
index.md: scripts/generate-index.pl
	@perl scripts/generate-index.pl $(ALL_BLOG) > $@

$(addprefix $(DEPLOY_DIRECTORY),%.html): %.md template.tmp writ.min.css
	@echo Converting: $< $@
	@mkdir -p $(dir $@)
	$(MARKDOWN) $< --output $@

$(addprefix $(DEPLOY_DIRECTORY),%.css): %.css
	@echo Moving $< to $@
	@mkdir -p $(dir $@)
	@cp $< $@

$(addprefix $(DEPLOY_DIRECTORY),%.png): %.1
	@convert $< $@

$(addprefix $(DEPLOY_DIRECTORY),%.jpg): %.jpg
	@echo "copy $< to $@"
	@cp $< $@

$(addprefix $(DEPLOY_DIRECTORY),%.css): %.css
	@cp $< $@

%.1: %.mp
	@(cd `dirname $<`;mpost `basename $<`)


deploy: bake save
	(cd deploy && git reset --soft v1.0 && git add . && git commit -m 'deploy'  && git push --force -u github master)

save:
	git add . ; git commit -m 'save'; git push origin pandoc
t:
	echo $(ALL_MD)  $(HTML_FROM_MD)
