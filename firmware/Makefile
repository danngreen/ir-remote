BUILDDIR := build

all: | $(BUILDDIR)
	cmake --build $(BUILDDIR)

$(BUILDDIR):
	cmake -B $(BUILDDIR) -GNinja

clean:
	rm -rf $(BUILDDIR)

