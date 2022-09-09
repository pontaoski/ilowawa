#include <QMenu>
#include <QStandardPaths>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KStandardAction>
#include <QItemSelectionModel>
#include <QGuiApplication>
#include <QClipboard>
#include <KIO/PasteJob>

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
