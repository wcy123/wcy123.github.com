TEMPLATE = ./template.tmp
CSS = writ.min.css
DEPLOY = deploy
# Rule for converting github flavored markdown to html5
MARKDOWN := rustdoc --output deploy/blog


#
DEPLOY_DIRECTORY = ./$(DEPLOY)/
ALL_MP = $(shell find blog -type f -and -iname '*.mp')

PNG_FROM_MP := $(patsubst %.mp, $(DEPLOY_DIRECTORY)%.png,$(ALL_MP))

ALL_SRC_JEPG := $(shell find blog -type f -and -iname '*.jpg')
ALL_DST_JEPG := $(patsubst %.jpg, $(DEPLOY_DIRECTORY)%.jpg,$(ALL_SRC_JEPG))
OTHER += $(DEPLOY_DIRECTORY)writ.min.css $(ALL_DST_JEPG)


# First recipe is default. Nothing to do except dependency on all html files.
bake: generate $(PNG_FROM_MP) $(OTHER)

clean:
	rm -rf $(DEPLOY_DIRECTORY) `find . -iname '*.1' -or -iname 'mptextmp.tmp'`

# Recipe for html files in the deploy directory for a corresponding markdown
# file

.PHONY: generate
generate:
	# perl scripts/generate-index.pl
	#perl scripts/generate-index-mdbook.pl
	#perl scripts/generate-html.pl
	#$(MARKDOWN) index.md
	#$(MARKDOWN) daughter.md
	mdbook test && mdbook build

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
	bash deploy.sh

save:
	git add . ; git commit -m 'save'; git push origin pandoc
t:
	echo $(ALL_MD)  $(HTML_FROM_MD)
