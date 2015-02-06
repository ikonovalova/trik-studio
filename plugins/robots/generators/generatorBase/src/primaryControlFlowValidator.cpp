#include "generatorBase/primaryControlFlowValidator.h"

#include "src/threadsValidator.h"

using namespace generatorBase;
using namespace qReal;

/// @todo: Unify code with interpreter

PrimaryControlFlowValidator::PrimaryControlFlowValidator(
		qrRepo::RepoApi const &repo
		, ErrorReporterInterface &errorReporter
		, GeneratorCustomizer &customizer
		, Id const &diagramId
		, QObject *parent)
	: QObject(parent)
	, RobotsDiagramVisitor(repo, customizer)
	, mRepo(repo)
	, mErrorReporter(errorReporter)
	, mCustomizer(customizer)
	, mDiagram(diagramId)
{
}

bool PrimaryControlFlowValidator::validate(const QString &threadId)
{
	mIfBranches.clear();
	mLoopBranches.clear();

	findInitialNode();
	if (mInitialNode.isNull()) {
		error(QObject::tr("There is nothing to generate, diagram doesn't have Initial Node"), mDiagram);
		return false;
	}

	ThreadsValidator threadsValidator(mRepo, mCustomizer, mErrorReporter);
	if (threadsValidator.validate(mInitialNode, threadId)) {
		mErrorsOccured = false;
		startSearch(mInitialNode);
	} else {
		mErrorsOccured = true;
	}

	return !mErrorsOccured;
}

qReal::Id PrimaryControlFlowValidator::initialNode() const
{
	return mInitialNode;
}

QPair<LinkInfo, LinkInfo> PrimaryControlFlowValidator::ifBranchesFor(qReal::Id const &id) const
{
	return mIfBranches[id];
}

QPair<LinkInfo, LinkInfo> PrimaryControlFlowValidator::loopBranchesFor(qReal::Id const &id) const
{
	return mLoopBranches[id];
}

void PrimaryControlFlowValidator::visitRegular(Id const &id
		, QList<LinkInfo> const &links)
{
	if (links.size() != 1) {
		error(QObject::tr("This element must have exactly ONE outgoing link"), id);
	} else {
		checkForConnected(links[0]);
	}
}

void PrimaryControlFlowValidator::visitFinal(Id const &id
		, QList<LinkInfo> const &links)
{
	if (!links.isEmpty()) {
		error(QObject::tr("Final node must not have outgoing links"), id);
	}
}

void PrimaryControlFlowValidator::visitConditional(Id const &id
		, QList<LinkInfo> const &links)
{
	if (links.size() != 2) {
		error(QObject::tr("If block must have exactly TWO outgoing links"), id);
		return;
	}

	// In correct case exactly 2 of this 3 would be non-null
	LinkInfo const *trueLink = nullptr;
	LinkInfo const *falseLink = nullptr;
	LinkInfo const *nonMarkedLink = nullptr;

	for (LinkInfo const &link : links) {
		checkForConnected(link);

		switch (guardOf(link.linkId)) {
		case trueGuard:
			if (trueLink) {
				error(QObject::tr("Two outgoing links marked with 'true' found"), id);
				return;
			} else {
				trueLink = &link;
			}
			break;

		case falseGuard:
			if (falseLink) {
				error(QObject::tr("Two outgoing links marked with 'false' found"), id);
				return;
			} else {
				falseLink = &link;
			}
			break;

		default:
			if (nonMarkedLink) {
				error(QObject::tr("There must be at least one link with \"true\" or \"false\" marker on it"), id);
				return;
			} else {
				nonMarkedLink = &link;
			}
			break;
		}
	}

	// Now we have correctly linked branches. Determining who is who...
	QPair<LinkInfo, LinkInfo> branches;
	if (!trueLink) {
		branches.first = *nonMarkedLink;
		branches.second = *falseLink;
	} else if (!falseLink) {
		branches.first = *trueLink;
		branches.second = *nonMarkedLink;
	} else {
		branches.first = *trueLink;
		branches.second = *falseLink;
	}

	mIfBranches[id] = branches;
}

void PrimaryControlFlowValidator::visitLoop(Id const &id
		, QList<LinkInfo> const &links)
{
	if (links.size() != 2) {
		error(QObject::tr("Loop block must have exactly TWO outgoing links"), id);
		return;
	}

	// In correct case must be non-null and different
	LinkInfo const *iterationLink = nullptr;
	LinkInfo const *nonMarkedBlock = nullptr;

	for (LinkInfo const &link : links) {
		checkForConnected(link);

		switch (guardOf(link.linkId)) {
		case iterationGuard:
			if (iterationLink) {
				error(QObject::tr("Two outgoing links marked with \"iteration\" found"), id);
				return;
			} else {
				iterationLink = &link;
			}
			break;
		default:
			if (nonMarkedBlock) {
				error(QObject::tr("There must be a link with \"iteration\" marker on it"), id);
				return;
			} else {
				nonMarkedBlock = &link;
			}
			break;
		}
	}

	if (iterationLink->target == nonMarkedBlock->target) {
		error(QObject::tr("Outgoing links from loop block must be connected to different blocks"), id);
	}

	mLoopBranches[id] = qMakePair(*iterationLink, *nonMarkedBlock);
}

void PrimaryControlFlowValidator::visitSwitch(Id const &id
		, QList<LinkInfo> const &links)
{
	QSet<QString> branches;
	bool defaultBranchFound = false;

	if (links.size() < 2) {
		error(QObject::tr("There must be at list TWO links outgoing from switch block"), id);
		return;
	}

	for (LinkInfo const &link : links) {
		checkForConnected(link);

		QString const condition = mRepo.property(link.linkId, "Guard").toString();
		if (condition.isEmpty()) {
			if (defaultBranchFound) {
				error(QObject::tr("There must be exactly one link without marker on it (default branch)"), id);
				return;
			} else {
				defaultBranchFound = true;
			}
		} else {
			if (branches.contains(condition)) {
				error(QObject::tr("Duplicate case branch: '%1'").arg(condition), link.linkId);
				return;
			}

			branches << condition;
		}
	}

	if (!defaultBranchFound) {
		error(QObject::tr("There must be a link without marker on it (default branch)"), id);
	}
}

void PrimaryControlFlowValidator::visitUnknown(Id const &id
		, QList<LinkInfo> const &links)
{
	Q_UNUSED(links)
	error(QObject::tr("Unknown block type"), id);
}

void PrimaryControlFlowValidator::visitFork(const Id &id, QList<LinkInfo> &links)
{
	Q_UNUSED(id)
	Q_UNUSED(links)
}

void PrimaryControlFlowValidator::visitJoin(const Id &id, QList<LinkInfo> &links)
{
	Q_UNUSED(id)
	Q_UNUSED(links)
}

void PrimaryControlFlowValidator::error(QString const &message, qReal::Id const &id)
{
	mErrorReporter.addError(message, id);

	// Returns false for possibility of one-line 'return error(...);'
	mErrorsOccured = true;
}

bool PrimaryControlFlowValidator::checkForConnected(LinkInfo const &link)
{
	if (!link.connected) {
		error(QObject::tr("Outgoing link is not connected"), link.linkId);
		return false;
	}

	return true;
}

void PrimaryControlFlowValidator::findInitialNode()
{
	qReal::IdList const diagramNodes(mRepo.children(mDiagram));
	for (qReal::Id const &diagramNode : diagramNodes) {
		if (mCustomizer.isInitialNode(diagramNode)) {
			mInitialNode = mRepo.logicalId(diagramNode);
			return;
		}
	}

	mInitialNode = Id();
}
