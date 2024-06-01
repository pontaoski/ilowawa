#include <QMenu>
#include <QStandardPaths>

#include "document.h"

struct SDocument::Private
{
	SWindow* window;
};

SDocument::SDocument(SWindow* parent) : QObject(parent), d(new Private)
{
	d->window = parent;
}

SDocument::~SDocument()
{

}

SWindow* SDocument::window() const
{
	return d->window;
}

void SDocument::moveTo(SWindow* window)
{
	d->window->transferDocumentTo(this, window);
}
